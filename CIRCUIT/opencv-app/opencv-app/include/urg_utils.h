#ifndef URG_UTILS_H
#define URG_UTILS_H

/*!
  \file
  \brief URG sensor utility

  \author Satofumi KAMIMURA

  $Id$
*/

#ifdef __cplusplus
extern "C" {
#endif

#include "urg_sensor.h"


    /*!
      \brief Returns the string message for the last URG error

      \param[in] urg URG control structure

      \retval String message for the last URG error

      Example
      \code
      if (!urg_open(&urg, "/dev/ttyACM0", 115200, URG_SERIAL)) {
      printf("urg_open: %s\n", urg_error(&urg));
      return -1;
      } \endcode
    */
    extern const char *urg_error(const urg_t *urg);


    /*!
      \brief Obtains the minimum and maximum distance values from sensor measurements

      \param[in] urg URG control structure
      \param[out] min_distance minimum distance [mm]
      \param[out] max_distance maximum distance [mm]

      Example
      \code
      long min_distance, max_distance;
      urg_distance_min_max(&urg, &min_distance, &max_distance);

      for (int i = 0; i < n; ++i) {
      long distance = data[i];
      if ((distance < min_distance) || (distance > max_distance)) {
      continue;
      }
      ...
      } \endcode
    */
    extern void urg_distance_min_max(const urg_t *urg,
                                     long *min_distance, long *max_distance);


    /*!
      \brief Gets the minimum and maximum step numbers

      Returns the minimum step and maximum step values as configured using urg_set_scanning_parameter()

      \param[in] urg URG control structure
      \param[out] min_step minimum step
      \param[out] max_step maximum step

      As seen from the top of the sensor: the frontal step is 0, going counter-clockwise are positive values, going clockwise are negative values.

      \image html sensor_step_image.png shows the relation between sensor and steps

      The actual values for min_step, max_step change with sensor type/series.

      Example
      \code
      urg_step_min_max(&urg, &min_step, &max_step);

      printf("range first: %d [deg]\n", urg_step2deg(&urg, min_step));
      printf("range last : %d [deg]\n", urg_step2deg(&urg, max_step)); \endcode

      \see urg_set_scanning_parameter(), urg_step2rad(), urg_step2deg()
    */
    extern void urg_step_min_max(const urg_t *urg, int *min_step, int *max_step);


    /*!
       \brief Returns the time [usec] for 1 scan
    */
    extern long urg_scan_usec(const urg_t *urg);


    /*!
       \brief Returns the maximum size of data received from the sensor
    */
    extern int urg_max_data_size(const urg_t *urg);


    /*!
      \brief Converts index to angle in radians

      Index is the position of each measurement data in the array returned using urg_get_distance().
      This function applies to the last array of measurement data read from the sensor.

      \param[in] urg URG control structure
      \param[in] index index value

      \return Angle [radian]

      The index value depends on the start/end steps configuration used for measurement.

      \image html sensor_index_image.png shows the relation between start/end step configuration and index

      Example
      \code
      int n = urg_get_distance(&urg, data, NULL);
      for (int i = 0; i < n; ++i) {
      long distance = data[i];
      double radian = urg_index2rad(i);
      double x = distance * cos(radian);
      double y = distance * sin(radian);
      printf("%.1f, %.1f\n", x, y);
      } \endcode

      \see urg_index2deg(), urg_rad2index(), urg_deg2index()
    */
    extern double urg_index2rad(const urg_t *urg, int index);


    /*!
       \brief Converts index to angle in degrees
    */
    extern double urg_index2deg(const urg_t *urg, int index);


    /*!
       \brief Converts angle in radians to index
    */
    extern int urg_rad2index(const urg_t *urg, double radian);


    /*!
       \brief Converts angle in degrees to index
    */
    extern int urg_deg2index(const urg_t *urg, double degree);


    /*!
      \brief Converts angle in radians to step number

      Conversion to angle (radian) is performed according to the min/max step definition using urg_step_min_max().

      \param[in] urg URG control structure
      \param[in] radian angle [radian]

      \return step value

      \image html sensor_angle_image.png shows the relation between steps and angles

      When the conversion from angle to step results on a fractional number, the value is rounded down towards zero (floor).

      \see urg_step_min_max(), urg_deg2step(), urg_step2rad(), urg_step2deg()
    */
    extern int urg_rad2step(const urg_t *urg, double radian);


    /*!
       \brief Converts angle in degrees to step number
    */
    extern int urg_deg2step(const urg_t *urg, double degree);


    /*!
       \brief Converts step number to angle in radians
    */
    extern double urg_step2rad(const urg_t *urg, int step);


    /*!
       \brief Converts step number to angle in degrees
    */
    extern double urg_step2deg(const urg_t *urg, int step);

    /*!
       \brief Converts step number to index
    */
    extern int urg_step2index(const urg_t *urg, int step);

    /*!
       \brief Wait at the specified time
    */
    extern void urg_delay(int delay_msec);
#ifdef __cplusplus
}
#endif

#endif /* !URG_UTILS_H */
