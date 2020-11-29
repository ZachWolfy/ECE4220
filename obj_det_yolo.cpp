#include <iostream>
#include <fstream>

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

const char* keys =
"{help h usage ? | | Usage examples: \n\t\t./object_detection_yolo.out --image=dog.jpg \n\t\t./object_detection_yolo.out --video=run_sm.mp4}"
"{image i        |<none>| input image   }"
"{video v       |<none>| input video   }";

using namespace std;
using namespace cv;
using namespace dnn;

float confThreshold = 0.5;	//confidence threshold
float nmsThreshold = 0.4;	//non-maximum suppression threshold
int inpWidth = 416;			//width of networks input image
int inpHeight = 416;		//height of networks input image
vector<string>classes;


// Get the names of the output layers
vector<String> getOutputsNames(const Net& net)
{
    static vector<String>names;
    if (names.empty())
    {
        //Get the indices of the output layers, i.e. the layers with unconnected outputs
        vector<int>outLayers = net.getUnconnectedOutLayers();
        
        //get the names of all the layers in the network
        vector<String>layersNames = net.getLayerNames();
        
        // Get the names of the output layers in names
        names.resize(outLayers.size());
        for (size_t i = 0; i < outLayers.size(); ++i)
        names[i] = layersNames[outLayers[i] - 1];
    }
    return names;
}

//remove bounding boxes with low confidence (non-maxima suppression)
void postprocess(Mat& frame, const vector<Mat>& out)
{
	vector<int>classID;
	vector<float>confidences;
	vector<Rect>boxes;
	
	for(size_t i = 0; i < out.size(); i++)
	{
		float *data - (float*)out[i].data;
		for(int j = 0; j < out[i].rows; j++, data += out[i].cols)
		{
			Mat scores = out[i].row(j).colRange(5,out[i].cols);
			Point classIDPoint;
			double confidence;
			//get value and location maximum scores
			minMaxLoc(scores, 0, &confidence, 0, &classIDPoint);
			if(confidence > confThreshold)
			{
				int midX = (int)(data[0] * frame.cols);
                int midY = (int)(data[1] * frame.rows);
                int width = (int)(data[2] * frame.cols);
                int height = (int)(data[3] * frame.rows);
                int left = midX - width / 2;
                int top = midY - height / 2;
				
				classID.push_back(classIDPoint.x);
				confidence.push_back((float)confidence);
				boxes.push_back(Rect(left,top,width,length);
			}
		}
	}
	
	//non-maximum suppression
	vector<int>indices;
	NMSBoxes(boxes,confidences, confThreshold, nmsThreshold, indices);
	for(size_t i = 0; i < indices.size(); i++)
	{
		int idx = indices[i];
		Rect box = boxes[idx];
		drawPred(classIds[idx], confidence[idx], box.x, box.y, box.x + box.width,
					box.y + box.height, frame);
	}
	
	return;
}

int main(int argc, char* argv)
{
	string classFile = "coco.names";
	ifstream ifs(classFile.c_str());
	string line;
	while(getline(ifs, line))
	{
		classes.push_back(line);
	}
	
	string modelConfig = "yolov3.cfg";
	string modelWeights = "yolov3.weights";
	
	//Load Darknet
	Net net = readNetFromDarknet(modelConfig, modelWeights);
	net.setPreferableBackend(DNN_BACKEND_OPENCV);
	net.setPreferableTarget(DN_TARGET_CPU);
	
	//open file
	string str, outputFile;
	try
	{
		outputFile = "yolo_out_cpp.avi";
		if(parser.has("image"))
		{
			str = parser.get<String>("image");
			ifstream ifile(str);
			if(!ifile)
			{
				throw("error");
			}
			cap.open(str);
			str.replace(str.end()-4, str.end(), "_yolo_out_cpp.jpg");
			outputFile = str;
		}
		
	}
	catch(...)
	{
		cout << "Could not open image" << endl;
		return 0;
	}
	
	
	//create a window
	static const string kWinName = "Object Detection";
	namedWindow(kWinName, WINDOW_NORMAL);
	
	//process frame
	while(waitkey(1) < 0)
	{
		cap >> frame;
		
		if(frame.empty())
		{
			cout << "Finish processing" << endl
				 << "Output file is << outputFile << endl;
				 
			waitKey(3000);
			break;
		}
		
		//create blob from a frame in 4D
		blobFromImage(frame, blob, 1/255.0, size(inpWidth, inpHeight), scalar(0,0,0), true, false);
		
		//sets input to darknet network
		net.setInput(blob);
		
		vector<Mat>out;
		net.forward(out, getOutputsNames(net));
		
		//remove bounding boxes with low confidence
		postprocess(frame, out);
		
		//for efficency getting the overall time
		vector<double>layersTimes;
        double freq = getTickFrequency() / 1000;
        double t = net.getPerfProfile(layersTimes) / freq;
        string label = format("Inference time for a frame : %.2f ms", t);
        putText(frame, label, Point(0, 15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255));
		
		//write the frame with detection boxes
		Mat detectedFrame;
		frame.convertTo(detectedFrame, CV_8U);
		if (parser.has("image"))
		{
			imwrite(outputFile, detectedFrame);
		}

		imshow(kWinName, frame);
		
	}
	
	cap.release();
	
	return 0;
}