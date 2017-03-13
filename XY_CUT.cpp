#include <opencv2\opencv.hpp>
#include <vector>
using namespace std;
using namespace cv;

vector<Rect> XYCut_projH(const Mat1b& src, Rect roi)
{
	Mat1b projH;
	reduce(src(roi), projH, 1, CV_REDUCE_MAX);

	vector<Rect> rects;

	bool bOut = true;
	vector<int> coords;
	coords.push_back(0);

	for (int i = 0; i < projH.rows; ++i)
	{
		if (bOut && projH(i) > 0)
		{
			coords.back() = (coords.back() + i) / 2;
			bOut = false;
		}
		else if (!bOut && projH(i) == 0)
		{
			coords.push_back(i);
			bOut = true;
		}
	}

	coords.front() = 0;
	coords.back() = projH.rows;
	if (coords.size() <= 1) return rects;

	for (int i = 0; i < coords.size() - 1; ++i)
	{
		Rect r(0, coords[i], src.cols, coords[i + 1] - coords[i]);
		r = (r + roi.tl()) & roi;
		rects.push_back(r);
	}
	return rects;
}

vector<Rect> XYCut_projV(const Mat1b& src, Rect roi)
{
	Mat1b projV;
	reduce(src(roi), projV, 0, CV_REDUCE_MAX);

	vector<Rect> rects;

	bool bOut = true;
	vector<int> coords;
	coords.push_back(0);

	for (int i = 0; i < projV.cols; ++i)
	{
		if (bOut && projV(i) > 0)
		{
			coords.back() = (coords.back() + i) / 2;
			bOut = false;
		}
		else if (!bOut && projV(i) == 0)
		{
			coords.push_back(i);
			bOut = true;
		}
	}

	coords.front() = 0;
	coords.back() = projV.cols;
	if (coords.size() <= 1) return rects;

	for (int i = 0; i < coords.size() - 1; ++i)
	{
		Rect r(coords[i], 0, coords[i + 1] - coords[i], src.rows);
		r = (r + roi.tl()) & roi;
		rects.push_back(r);
	}
	return rects;
}

void XYCut_step(const Mat1b& src, Rect roi, vector<Rect>& rects, bool bAlternate)
{
	vector<Rect> step;
	if (bAlternate)
	{
		step = XYCut_projH(src, roi);

		if ((step.size() == 1) && (step[0] == roi) && (XYCut_projV(src, roi).size() == 1))
		{
			rects.push_back(roi);
			return;
		}
	}
	else
	{
		step = XYCut_projV(src, roi);

		if ((step.size() == 1) && (step[0] == roi) && (XYCut_projH(src, roi).size() == 1))
		{
			rects.push_back(roi);
			return;
		}
	}

	for (int i = 0; i < step.size(); ++i)
	{
		XYCut_step(src, step[i], rects, !bAlternate);
	}
}

void XYCut(const Mat1b& src, vector<Rect>& rects)
{
	bool bAlternate = true;
	Rect roi(0, 0, src.cols, src.rows);

	XYCut_step(src, roi, rects, bAlternate);
}
Mat GetSquareImage(const Mat& img, int target_width = 500)
{
	int width = img.cols,
		height = img.rows;

	Mat square = Mat::zeros(target_width, target_width, img.type());

	int max_dim = (width >= height) ? width : height;
	float scale = ((float)target_width) / max_dim;
	Rect roi;
	if (width >= height)
	{
		roi.width = target_width;
		roi.x = 0;
		roi.height = height * scale;
		roi.y = (target_width - roi.height) / 2;
	}
	else
	{
		roi.y = 0;
		roi.height = target_width;
		roi.width = width * scale;
		roi.x = (target_width - roi.width) / 2;
	}

	resize(img, square(roi), roi.size());

	return square;
}

//int main()
//{
//	Mat img = imread("C:\\TableRecognition\\Data\\unlv-table-png\\0110_099.png", IMREAD_GRAYSCALE);
//	// invert image, if needed
//	img = ~img;
//	
//	// Apply (modified) XY Cut
//	vector<Rect> rects;
//	XYCut(img, rects);
//
//	// Show results
//	Mat res;
//	Mat org;
//	cvtColor(img, res, CV_GRAY2BGR);
//	cvtColor(img, org, CV_GRAY2RGB);
//	for (int i = 0; i < rects.size(); ++i)
//	{
//		rectangle(res, rects[i], Scalar(0, 0, 255));
//	}
//	
//	// displaying origional image and the output image in the same window
//	Size sz1 = img.size(); // size of origional image
//	Size sz2 = res.size();	// size of resultant image
//	
//	// adjusting the width and height of the output window
//	Mat im3(sz1.height, sz1.width + sz2.width, CV_8UC3);
//	
//	
//	// writing origionl image to the left of the output window
//	Mat left(im3, Rect(0, 0, sz1.width, sz1.height));
//	org.copyTo(left); 
//	
//	// writing resultant image to the right of the output window
//	Mat right(im3, Rect(sz1.width, 0, sz2.width, sz2.height));
//	res.copyTo(right);
//
//	// adjusting the responsiveness of the window
//	namedWindow("im3", CV_WINDOW_NORMAL);
//	resizeWindow("im3", 800, 500);
//	imshow("im3", im3);
//	
//	waitKey();
//	return 0;
//}
//
//#include <opencv2\opencv.hpp>
//#include <vector>
//using namespace std;
//using namespace cv;
//
//int main()
//{
//	Mat1b img = imread("C:\\TableRecognition\\Data\\unlv-table-png\\0110_099.png", IMREAD_GRAYSCALE);
//
//	Mat1f dist;
//	Mat1i labels;
//	distanceTransform(img, dist, labels, CV_DIST_L2, 3, DIST_LABEL_CCOMP);
//
//	// Show result
//
//	Mat1b labels1b;
//	labels.convertTo(labels1b, CV_8U);
//	normalize(labels1b, labels1b, 0, 255, NORM_MINMAX);
//	Mat3b res;
//	applyColorMap(labels1b, res, COLORMAP_JET);
//	res.setTo(Scalar(0, 0, 0), ~img);
//
//	imshow("Result", res);
//	waitKey();
//
//	return 0;
//}