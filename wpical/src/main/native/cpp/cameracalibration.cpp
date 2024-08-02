#include <cameracalibration.h>
#include <iostream>

nlohmann::json cameracalibration::calibrate(const std::string &input_video, float square_width, float marker_width, int board_width, int board_height)
{
    std::cout << "test print" << std::endl;
    // Aruco Board
    cv::aruco::Dictionary aruco_dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_5X5_1000);
    cv::Ptr<cv::aruco::CharucoBoard> charuco_board = new cv::aruco::CharucoBoard(cv::Size(board_width, board_height), square_width * 0.0254, marker_width * 0.0254, aruco_dict);
    cv::aruco::CharucoDetector charuco_detector(*charuco_board);

    // Video capture
    cv::VideoCapture video_capture(input_video);
    int frame_count = 0;
    cv::Size frame_shape;

    // Detection output
    std::vector<cv::Point2f> all_corners;
    std::vector<int> all_ids;
    std::vector<int> all_counter;

    while (video_capture.isOpened())
    {
        cv::Mat frame;
        video_capture >> frame;

        if (frame.empty())
        {
            break;
        }

        // Limit FPS
        frame_count++;

        if (frame_count % 10 != 0)
        {
            continue;
        }

        // Detect
        cv::Mat frame_gray;
        cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);

        frame_shape = frame_gray.size();

        std::vector<cv::Point2f> charuco_corners;
        std::vector<int> charuco_ids;
        std::vector<std::vector<cv::Point2f>> marker_corners;
        std::vector<int> marker_ids;

        charuco_detector.detectBoard(frame_gray, charuco_corners, charuco_ids, marker_corners, marker_ids);

        cv::Mat debug_image = frame;

        if (!charuco_ids.empty() && charuco_ids.size() >= 4)
        {
            all_corners.insert(all_corners.end(), charuco_corners.begin(), charuco_corners.end());
            all_ids.insert(all_ids.end(), charuco_ids.begin(), charuco_ids.end());
            all_counter.push_back(charuco_ids.size());

            cv::aruco::drawDetectedMarkers(debug_image, marker_corners, marker_ids);
            cv::aruco::drawDetectedCornersCharuco(debug_image, charuco_corners, charuco_ids);
        }

        cv::imshow("Frame", debug_image);
        if (cv::waitKey(1) == 'q')
        {
            break;
        }
    }

    video_capture.release();
    cv::destroyAllWindows();

    // Calibrate
    cv::Mat camera_matrix, dist_coeffs;
    std::vector<cv::Mat> r_vecs, t_vecs;
    std::vector<double> std_dev_intrinsics, std_dev_extrinsics, per_view_errors;

    std::vector<std::vector<cv::Point2f>> corners = {all_corners};
    std::vector<std::vector<int>> ids = {all_ids};

    cv::aruco::calibrateCameraCharuco(
        corners, ids, charuco_board, frame_shape, camera_matrix, dist_coeffs,
        r_vecs, t_vecs, std_dev_intrinsics, std_dev_extrinsics, per_view_errors, cv::CALIB_RATIONAL_MODEL);

    std::cout << "Completed camera calibration" << std::endl;

    // Save calibration output
    nlohmann::json camera_model = {
        {"camera_matrix", std::vector<double>(camera_matrix.begin<double>(), camera_matrix.end<double>())},
        {"distortion_coefficients", std::vector<double>(dist_coeffs.begin<double>(), dist_coeffs.end<double>())},
        {"avg_reprojection_error", cv::mean(per_view_errors)[0]}};

    return camera_model;
}

nlohmann::json cameracalibration::calibrate(const std::string &input_video, float square_width, int board_width, int board_height, double imagerWidthPixels, double imagerHeightPixels, double focal_length_guess)
{
    // Video capture
    cv::VideoCapture video_capture(input_video);
    int frame_count = 0;

    // Detection output
    std::vector<std::vector<cv::Point2f>> all_corners;
    std::vector<std::vector<mrcal_point3_t>> all_points;

    // Dimensions
    cv::Size boardSize(board_width - 1, board_height - 1);
    cv::Size imagerSize(imagerWidthPixels, imagerHeightPixels);

    while (video_capture.isOpened())
    {
        cv::Mat frame;
        video_capture >> frame;

        if (frame.empty())
        {
            break;
        }

        // Detect
        cv::Mat frame_gray;
        cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);

        cv::Mat debug_image = frame;

        std::vector<cv::Point2f> corners;

        bool found = cv::findChessboardCorners(frame_gray, boardSize, corners, cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE);

        cv::drawChessboardCorners(frame, boardSize, corners, found);

        if (found)
        {
            all_corners.push_back(corners);
        }

        // Display the frame with detected corners
        cv::imshow("Checkerboard Detection", frame);

        // Exit loop if 'q' is pressed
        if (cv::waitKey(30) == 'q')
        {
            break;
        }
    }

    video_capture.release();
    cv::destroyAllWindows();

    for (int i = 0; i < all_corners.size(); i++)
    {
        std::vector<mrcal_point3_t> points;
        for (int j = 0; j < all_corners.at(i).size(); j++)
        {
            points.push_back(mrcal_point3_t{all_corners.at(i).at(j).x, all_corners.at(i).at(j).y, 1.0f});
        }
        all_points.push_back(points);
    }

    std::vector<mrcal_point3_t> observations_board;
    std::vector<mrcal_pose_t> frames_rt_toref;

    for (int i = 0; i < all_points.size(); i++)
    {
        size_t total_points = board_width * boardSize.height * all_points.at(i).size();
        observations_board.reserve(total_points);
        frames_rt_toref.reserve(all_points.at(i).size());

        auto ret = getSeedPose(all_points.at(i).data(), boardSize, imagerSize, square_width * 0.0254, focal_length_guess);

        observations_board.insert(observations_board.end(), all_points.at(i).begin(), all_points.at(i).end());
        frames_rt_toref.push_back(ret);
    }

    auto cal_result = mrcal_main(observations_board, frames_rt_toref, boardSize, square_width * 0.0254, imagerSize, focal_length_guess);

    auto &stats = *cal_result;

    std::vector<double> cameraMatrix = {
        // fx 0 cx
        stats.intrinsics[0],
        0,
        stats.intrinsics[2],
        // 0 fy cy
        0,
        stats.intrinsics[1],
        stats.intrinsics[3],
        // 0 0 1
        0,
        0,
        1};

    std::vector<double> distortionCoefficients = {
        stats.intrinsics[4],
        stats.intrinsics[5],
        stats.intrinsics[6],
        stats.intrinsics[7],
        stats.intrinsics[8],
        stats.intrinsics[9],
        stats.intrinsics[10],
        stats.intrinsics[11],
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        0.0};

    nlohmann::json result = {
        {"camera_matrix", cameraMatrix},
        {"distortion_coefficients", distortionCoefficients},
        {"avg_reprojection_error", stats.rms_error}};

    return result;
}