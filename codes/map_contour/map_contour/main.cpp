#include <iostream>
#include <opencv2\opencv.hpp>
#include <vector>
#include <fstream>

int main()
{
	int smooth = 1; //Smooth radius for each contour line
	int space = 5; //The count of points between each sampling points

	int map_width = 750; //Output map width
	int map_height = 630; //Output map height

	cv::Mat img = cv::imread("map_desat_wb2_inv_blur2_frame.png"); //Input binary image
	std::ofstream file("map.txt", std::ofstream::out); //Output map file, v denodes vertex and l denodes loop

	//Remove other color chanels
	cv::Mat gray;
	cv::cvtColor(img, gray, CV_BGR2GRAY);

	int rows = gray.rows;
	int cols = gray.cols;
	std::vector<std::vector<cv::Point> > contours;
	cv::Mat edge_points(rows, cols, gray.type());

	//Extract precise contours image
	for (int i = 0; i < rows; i++)
	{
		const uchar *gray_data = gray.ptr<uchar>(i);
		uchar *edge_data = edge_points.ptr<uchar>(i);
		for (int j = 0; j < cols; j++)
		{
			int all_ones = 1;
			if (gray_data[j] > 0)
			{
				int up = i - 1;
				int down = i + 1;
				int left = j - 1;
				int right = j + 1;

				int up_value = up >= 0 ? gray.ptr<uchar>(up)[j] : 0;
				int down_value = down < rows ? gray.ptr<uchar>(down)[j] : 0;
				int left_value = left >= 0 ? gray_data[left] : 0;
				int right_value = right < cols ? gray_data[right] : 0;

				all_ones = up_value & down_value & left_value & right_value;
			}
			if (all_ones == 0)
				edge_data[j] = 1;
			else
				edge_data[j] = 0;
		}
	}

	//Make the contours loop for contours image
	for (int i = 0; i < rows; i++)
	{
		uchar *edge_data = edge_points.ptr<uchar>(i);
		for (int j = 0; j < cols; j++)
		{
			if (edge_data[j] > 0)
			{
				std::vector<cv::Point> contour;
				int next_i = i;
				int next_j = j;
				bool finished;
				do
				{
					finished = true;
					edge_points.ptr<uchar>(next_i)[next_j] = 0;
					contour.push_back(cv::Point(next_j, next_i));
					int old_i = next_i;
					int old_j = next_j;
					for (int ni = old_i - 1; ni <= old_i + 1; ni++)
					for (int nj = old_j - 1; nj <= old_j + 1; nj++)
					{
						if (ni >= 0 && ni < rows &&
							nj >= 0 && nj < cols &&
							edge_points.ptr<uchar>(ni)[nj] == 1)
						{
							next_i = ni;
							next_j = nj;
							finished = false;
						}
					}
				} while (!finished);
				contours.push_back(contour);
			}
		}
	}

	//Smooth the contours loop
	std::vector<std::vector<cv::Point> > smooth_contours;
	for (int i = 0; i < contours.size(); i++)
	{
		std::vector<cv::Point> &contour = contours[i];
		std::vector<cv::Point> smooth_contour;
		for (int j = 0; j < contour.size(); j++)
		{
			double total_x = 0;
			double total_y = 0;
			int total_c = 0;
			for (int k = j - smooth; k <= j + smooth; k++)
			{
				if (k >= 0 && k < contour.size())
				{
					total_x += contour[k].x;
					total_y += contour[k].y;
					total_c++;
				}
			}
			smooth_contour.push_back(cv::Point(total_x / total_c, total_y / total_c));
		}
		if (smooth_contour.size() > 2 * space + 1)
			smooth_contours.push_back(smooth_contour);
	}

	//Export contours to map file
	for (int i = 1; i < smooth_contours.size(); i++)
	for (int j = 0; j < smooth_contours[i].size(); j += space)
		file << "v "
		<< smooth_contours[i][j].x * ((map_width - 20.0) / gray.cols) + 10.0 << " "
		<< (gray.rows - smooth_contours[i][j].y) * ((map_height - 20.0)/ gray.rows) + 10.0 << std::endl;
	int index = 0;
	for (int i = 1; i < smooth_contours.size(); i++)
	{
		file << "l";
		for (int j = 0; j < smooth_contours[i].size(); j += space)
			file << " " << index++;
		file << std::endl;
	}
	return 0;
}