#ifndef URG_SENSOR_H
#define URG_SENSOR_H

/*!
  \file
  \brief URG sensor control

  Provides the basic functions for URG

  \author Satofumi KAMIMURA

  $Id$
*/

#ifdef __cplusplus
extern "C" {
#endif

#include "urg_connection.h"

    /*!
      \brief Measurement types
    */
    typedef enum {
        URG_DISTANCE,           //!<  Distance (range)
        URG_DISTANCE_INTENSITY, //!<  Distance (range) and intensity (strength)
        URG_MULTIECHO,          //!<  Multiecho distance
        URG_MULTIECHO_INTENSITY, //!<  Multiecho distance and intensity
        URG_STOP,                //!<  Stop measurement
        URG_UNKNOWN,             //!<  Unknown measurement type
    } urg_measurement_type_t;

    /*!
      \brief Distance data encoding types (number of bytes)
    */
    typedef enum {
        URG_COMMUNICATION_3_BYTE, //!<  Use 3-bytes encoding for distance
        URG_COMMUNICATION_2_BYTE, //!<  Use 2-bytes encoding for distance
    } urg_range_data_byte_t;


    enum {
        URG_SCAN_INFINITY = 0,  //!<  Continuous data scanning
        URG_MAX_ECHO = 3, //!<  Maximum number of echoes
    };


    /*!
       \brief Error handler
    */
    typedef urg_measurement_type_t
    (*urg_error_handler)(const char *status, void *urg);


    /*!
      \brief URG sensor control structure
    */
    typedef struct
    {
        int is_active;
        int last_errno;
        urg_connection_t connection;

        int first_data_index;
        int last_data_index;
        int front_data_index;
        int area_resolution;
        long scan_usec;
        int min_distance;
        int max_distance;
        int scanning_first_step;
        int scanning_last_step;
        int scanning_skip_step;
        int scanning_skip_scan;
        urg_range_data_byte_t range_data_byte;

        int timeout;
        int specified_scan_times;
        int scanning_remain_times;
        int is_laser_on;

        int received_first_index;
        int received_last_index;
        int received_skip_step;
        urg_range_data_byte_t received_range_data_byte;
        int is_sending;

        urg_error_handler error_handler;

        char return_buffer[80];
    } urg_t;


    /*!
      \brief Connect

      Connects to the given device and enables measurement

      \param[in,out] urg URG control structure
      \param[in] connection_type Type of the connection
      \param[in] device_or_address Name of the device
      \param[in] baudrate_or_port Connection baudrate [bps] or TCP/IP port number

      \retval 0 Successful
      \retval <0 Error

      The following values can be used in connection_type:

      - #URG_SERIAL
      - Serial, USB connection

      - #URG_ETHERNET
      - Ethernet connection
      Example
      \code
      urg_t urg;

      if (urg_open(&urg, URG_SERIAL, "/dev/ttyACM0", 115200) < 0) {
      return 1;
      }

      ...

      urg_close(&urg); \endcode

      \attention Call this function before using any other function on the URG library.

      \see urg_close()
    */
    extern int urg_open(urg_t *urg, urg_connection_type_t connection_type,
                        const char *device_or_address,
                        long baudrate_or_port);


    /*!
      \brief Disconnection

      Turns off the laser and closes the connection with the URG sensor.

      \param[in,out] urg URG control structure
      \see urg_open()
    */
    extern void urg_close(urg_t *urg);


    /*!
      \brief Defines the timeout value to use during communication

      \param[in,out] urg URG control structure
      \param[in] msec Timeout value [msec]

      \attention The urg_open() function always sets the timeout value to its default, if necessary call this function after urg_open().
    */
    extern void urg_set_timeout_msec(urg_t *urg, int msec);


    /*!
       \brief Starts the timestamp mode (time adjustment state)
    */
    extern int urg_start_time_stamp_mode(urg_t *urg);


    /*!
      \brief Read timestamp data

      \param[in,out] urg URG control structure

      \retval >=0 Timestamp value [msec]
      \retval <0 Error

      Example
      \code
      urg_start_time_stamp_mode(&urg);

      before_ticks = get_pc_msec_function();
      time_stamp = urg_time_stamp(&urg);
      after_ticks = get_pc_msec_function();

      // Processing of timestamp data
      ...

      urg_stop_time_stamp_mode(&urg); \endcode

      For a detailed use consult the \ref sync_time_stamp.c example
    */
    extern long urg_time_stamp(urg_t *urg);


    /*!
       \brief Stops the timestamp mode (returning to idle state)
    */
    extern int urg_stop_time_stamp_mode(urg_t *urg);


    /*!
      \brief Start getting distance measurement data

      Starts measurement data acquisition. The actual data can be retrieved using urg_get_distance(), urg_get_distance_intensity(), urg_get_multiecho(), urg_get_multiecho_intensity().

      \param[in,out] urg URG control structure
      \param[in] type Measurement type
      \param[in] scan_times Number of scans to request
      \param[in] skip_scan Interval between scans

      \retval 0 Successful
      \retval <0 Error

      The following values are possible for the type argument

      - #URG_DISTANCE ... Distance (range) data
      - #URG_DISTANCE_INTENSITY ... Distance (range) and intensity (strength) data
      - #URG_MULTIECHO ... Multiecho distance data
      - #URG_MULTIECHO_INTENSITY ... Multiecho distance and intensity data

      scan_times defines how many scans to capture from the sensor: >0 means a fixed number of scans, 0 or #URG_SCAN_INFINITY means continuous (infinite) scanning.
      To interrupt measurement at any time use urg_stop_measurement().

      skip_scan means, after obtaining one scan, skip measurement for the following X mirror (motor) revolutions.
      The values for skip_scan are from the range [0, 9].

      \image html skip_scan_image.png shows scan skip

      For example, for a sensor with one mirror (motor) revolution is 100 [msec] and skip_scan is set to 1, measurement data will be obtained with an interval of 200 [msec].

      Example
      \code
      enum { CAPTURE_TIMES = 10 };
      urg_start_measurement(&urg, URG_DISTANCE, CAPTURE_TIMES, 0);

      for (i = 0; i < CAPTURE_TIMES; ++i) {
      int n = urg_get_distance(&urg, data, &time_stamp);

      // Processing of obtained data
      ...
      } \endcode

      \see urg_get_distance(), urg_get_distance_intensity(), urg_get_multiecho(), urg_get_multiecho_intensity(), urg_stop_measurement()
    */
    extern int urg_start_measurement(urg_t *urg, urg_measurement_type_t type,
                                     int scan_times, int skip_scan);


    /*!
      \brief Gets distance data

      Receives distance data from the sensor. The urg_start_measurement() function was called beforehand with #URG_DISTANCE as type argument.

      \param[in,out] urg URG control structure
      \param[out] data Distance data array [mm]
      \param[out] time_stamp Timestamp [msec]

      \retval >=0 Number of data points received
      \retval <0 Error

      Distance data received from the sensor are stored in data array. data array should be previously allocated to hold all the data points requested from the sensor. To know how many data points are received, use the urg_max_data_size() function.

      time_stamp will hold the timestamp value stored on the sensor. When not necessary just pass NULL as argument.

      Example
      \code
      long *data = (long*)malloc(urg_max_data_size(&urg) * sizeof(data[0]));

      ...

      // Gets only measurement data
      urg_start_measurement(&urg, URG_DISTANCE, 1, 0);
      int n = urg_get_distance(&urg, data, NULL);

      ...

      // Gets measurement data and timestamp
      long time_stamp;
      urg_start_measurement(&urg, URG_DISTANCE, 1, 0);
      n = urg_get_distance(&urg, data, &time_stamp); \endcode

      \see urg_start_measurement(), urg_max_data_size()
    */
    extern int urg_get_distance(urg_t *urg, long data[], long *time_stamp);


    /*!
      \brief Gets distance and intensity data

      This is an extension to urg_get_distance() which allows to obtain also the intensity (strength) data. The urg_start_measurement() function was called beforehand with #URG_DISTANCE_INTENSITY as type argument.

      \param[in,out] urg URG control structure
      \param[out] data Distance data array [mm]
      \param[out] intensity Intensity data array
      \param[out] time_stamp Timestamp [msec]

      \retval >=0 Number of data points received
      \retval <0 Error

      Intensity data corresponds to the laser strength received during distance calculation. The characteristics of this value changes with sensor series. With some limitations, this property can be used to guess the reflectivity and color shade of an object.

      Regarding data and time_stamp arguments, refer to urg_get_distance().

      Intensity data received from the sensor are stored in intensity array. intensity array should be previously allocated to hold all the data points requested from the sensor. To know how many data points are received, use the urg_max_data_size() function.

      Example
      \code
      int data_size = urg_max_data_size(&urg);
      long *data = malloc(data_size * sizeof(long));
      long *intensity = malloc(data_size * sizeof(unsigned short));

      ...

      urg_start_measurement(&urg, URG_DISTANCE_INTENSITY, 1, 0);
      int n = urg_get_distance_intensity(&urg, data, intesnity, NULLL); \endcode

      \see urg_start_measurement(), urg_max_data_size()
    */
    extern int urg_get_distance_intensity(urg_t *urg, long data[],
                                          unsigned short intensity[],
                                          long *time_stamp);


    /*!
      \brief Gets distance data (multiecho mode)

      Receives multiecho distance data from the sensor. The urg_start_measurement() function was called beforehand with #URG_MULTIECHO as type argument.

      \param[in,out] urg URG control structure
      \param[out] data_multi Distance data array [mm]
      \param[out] time_stamp Timestamp [msec]

      \retval >=0 Number of data points received
      \retval <0 Error

      Multiecho means multiple range responses (echoes). For a single laser beam, multiple laser returns reflected from different targets may be received, and thus multiple range values are calculated.

      \image html multiecho_image.png shows multiecho measurement

      time_stamp will hold the timestamp value stored on the sensor, same as with urg_get_distance().

      The array data_multi will hold the multiecho range data, up to a maximum of #URG_MAX_ECHO (3) echoes per step. In case the echo does not exists then -1 will be stored on the array.

      \verbatim
      data_multi[0] ... step n range data (1st echo)
      data_multi[1] ... step n range data (2nd echo)
      data_multi[2] ... step n range data (3rd echo)
      data_multi[3] ... step (n + 1) range data (1st echo)
      data_multi[4] ... step (n + 1) range data (2nd echo)
      data_multi[5] ... step (n + 1) range data (3rd echo)
      ... \endverbatim

      In the array, the cells numbered (3n + 0) will hold the range data for first echo (same data as for the urg_get_distance() function), for the other cells (3n + 1) and (3n + 2) data is stored in descending order. \n
      This is, the order data_multi[3n + 1] >= data_multi[3n + 2] is assured, however the relation between data_multi[3n + 0] and data_multi[3n + 1] is not defined. (When data_multi[3n + 1] == data_multi[3n + 2] it means the echo does not exists and the stored value is -1.)

      Example
      \code
      long *data_multi = malloc(3 * urg_max_data_size(&urg) * sizeof(long));

      ...

      urg_start_measurement(&urg, URG_MULTIECHO, 1, 0);
      int n = urg_get_distance_intensity(&urg, data_multi, NULLL); \endcode

      \see urg_start_measurement(), urg_max_data_size()
    */
    extern int urg_get_multiecho(urg_t *urg, long data_multi[], long *time_stamp);


    /*!
      \brief Gets distance and intensity data (multiecho mode)

      This is an extension to urg_get_multiecho() which allows to obtain also the intensity (strength) data for multiple echoes. The urg_start_measurement() function was called beforehand with #URG_MULTIECHO_INTENSITY as type argument.

      \param[in,out] urg URG control structure
      \param[out] data_multi Distance data array [mm]
      \param[out] intensity_multi Intensity data array
      \param[out] time_stamp Timestamp [msec]

      \retval >=0 Number of data points received
      \retval <0 Error

      data_multi and time_stamp are as described in urg_get_multiecho() function.

      The order of data in the array intensity_multi is defined by how the data_multi array was sorted. The size of the intensity_multi can be obtained using urg_max_data_size().
      Example
      \code
      int data_size = urg_max_data_size(&urg);
      long *data_multi = malloc(3 * data_size * sizeof(long));
      long *intensity_multi = malloc(3 * data_size * sizeof(unsigned short));

      ...

      urg_start_measurement(&urg, URG_DISTANCE_INTENSITY, 1, 0);
      int n = urg_get_multiecho_intensity(&urg, data_multi,
      intesnity_multi, NULLL); \endcode

      \see urg_start_measurement(), urg_max_data_size()
    */
    extern int urg_get_multiecho_intensity(urg_t *urg, long data_multi[],
                                           unsigned short intensity_multi[],
                                           long *time_stamp);


    /*!
      \brief Stops measurement process and turns off the laser.

      It stops the measurement started with \ref urg_start_measurement() function.

      \param[in,out] urg URG control structure

      \retval 0 Successful
      \retval <0 Error

      Example
      \code
      urg_start_measurement(&urg, URG_DISTANCE, URG_SCAN_INFINITY, 0);
      for (int i = 0; i < 10; ++i) {
      urg_get_distance(&urg, data, NULL);
      }
      urg_stop_measurement(&urg); \endcode

      \see urg_start_measurement()
    */
    extern int urg_stop_measurement(urg_t *urg);


    /*!
      \brief Configure measurement parameters

      This function allows definining the scope (start and end steps) for measurement. The number of measurement data (steps) returned by urg_get_distance() and similar is defined here.

      \param[in,out] urg URG control structure
      \param[in] first_step start step number
      \param[in] last_step end step number
      \param[in] skip_step step grouping factor

      \retval 0 Successful
      \retval <0 Error

      Observing the sensor from the top, the step 0 corresponds to the very front of the sensor, steps at the left side (counter clockwise) of step 0 are positive numbers and those to the right side (clockwise) are negative numbers.

      \image html sensor_angle_image.png shows the relation between sensor and steps

      The spacing between steps, the minimum and maximum step numbers depend on the sensor. Use urg_step_min_max() to get the minimum and maximum step values.\n

      first_step and last_step define the data measurement scope ([first_step, last_step])B

      skip_step allows setting a step grouping factor, where valid values are [0, 99].\n
      With the skip_step parameter, several adjacent steps are grouped and combined into 1 single step, thus the amount of data transmitted from the sensor is reduced and so the response time of measurement data adquisition functions. Of course, grouping several steps into one means the measurement resolution is reduced.

      For example, for the following range data obtained in the sensor:
      \verbatim
      100, 101, 102, 103, 104, 105, 106, 107, 108, 109
      \endverbatim

      And setting skip_step to 2, the range data returned is:
      \verbatim
      100, 102, 104, 106, 108
      \endverbatim

      for each group, the smallest range value is returned.
      Example
      \code
      urg_set_scanning_parameter(&urg, urg_deg2step(&urg, -45),
      urg_deg2step(&urg, +45), 1);
      urg_start_measurement(&urg, URG_DISTANCE, 0);
      int n = urg_get_distance(&urg, data, NULL);
      for (int i = 0; i < n; ++i) {
      printf("%d [mm], %d [deg]\n", data[i], urg_index2deg(&urg, i));
      } \endcode

      \see urg_step_min_max(), urg_rad2step(), urg_deg2step()
    */
    extern int urg_set_scanning_parameter(urg_t *urg, int first_step,
                                          int last_step, int skip_step);

    /*!
      \brief Change the size (number of bytes) of measurement data used during communications.

      When receiving data from the sensor, changes the number of bytes used to represent measurement data.

      \param[in,out] urg URG control structure
      \param[in] data_byte Number of bytes used to represent measurement data

      \retval 0 Successful
      \retval <0 Error

      data_byte can be:

      - URG_COMMUNICATION_3_BYTE ... to represent data in 3 bytes
      - URG_COMMUNICATION_2_BYTE ... to represent data in 2 bytes

       \n
      The initial (default) data size is 3 bytes. If the number of bytes is changed to 2, the actual received message length becomes around 2/3 of the original length. However, using 2 bytes means the maximum measurement range is 4095, therefore use it only when measurement targets are 4 [m] from the sensor.
    */
    extern int urg_set_measurement_data_size(urg_t *urg,
                                               urg_range_data_byte_t data_byte);


    /*!
       \brief Turns on the laser
    */
    extern int urg_laser_on(urg_t *urg);


    /*!
       \brief Turns off the laser
    */
    extern int urg_laser_off(urg_t *urg);


    /*!
       \brief Reboots the sensor
    */
    extern int urg_reboot(urg_t *urg);


    /*!
      \brief Sets the sensor into low power mode (sleep state)

      During low power mode, the scanner motor stops and so measurement is interrupted.

      - Low power mode
        - Laser is turned off and so measurement is stopped
        - The scanner motor is stopped.

      To recover from low power mode call the function \ref urg_wakeup()
      \see urg_wakeup()
    */
    extern void urg_sleep(urg_t *urg);


    /*!
      \brief Returns from the low power mode (sleep state) to the normal mode (idle state)
      \see urg_sleep()
    */
    extern void urg_wakeup(urg_t *urg);

    /*!
      \brief Returns whether the sensor is stable to perform measurement or not

      \retval 1 The sensor can do measurement
      \retval 0 The sensor cannot do measurement

      Right after power on the motor rotation is not yet stable for measurement, or if any failure condition was detected,
      0 is returned.
    */
    extern int urg_is_stable(urg_t *urg);


    /*!
      \brief Returns the sensor model string

      Returns the string message corresponding to the sensor model. This message is sensor dependent.

      \param[in] urg URG control structure
      \return sensor model string
    */
    extern const char *urg_sensor_product_type(urg_t *urg);


    /*!
      \brief Returns the sensor serial number string

      Returns the string message corresponding to the sensor serial number. This message is sensor dependent.

      \param[in] urg URG control structure
      \return serial number string
    */
    extern const char *urg_sensor_serial_id(urg_t *urg);


    /*!
      \brief Returns the current sensor firmware version string

      Returns the string message corresponding to the current sensor firmware version. This message is sensor dependent.

      \param[in] urg URG control structure
      \return firmware version string
    */
    extern const char *urg_sensor_firmware_version(urg_t *urg);


    /*!
      \brief Returns the current sensor status string

      Returns the string message corresponding to the current sensor status. This message is sensor dependent.

      \param[in] urg URG control structure
      \return current sensor status string
    */
    extern const char *urg_sensor_status(urg_t *urg);


    /*!
      \brief Returns the current sensor state string

      Returns the string message corresponding to the current sensor state. This message is sensor dependent.

      \param[in] urg URG control structure
      \return current sensor state string

      \attention For details, please refer to the SCIP communication protocol specification.
    */
    extern const char *urg_sensor_state(urg_t *urg);


    /*!
      \brief Registers an error handler for measurement functions

      The error handler will be called for the Gx, Mx commands when the response code is not "00" or "99".
    */
    extern void urg_set_error_handler(urg_t *urg, urg_error_handler handler);


    /*!
      \brief Decodes a SCIP message

      \param[in] data the SCIP message to decode
      \param[in] size the data encoding types (number of bytes for encoding)

      \retval Value after decoding
    */
    extern long urg_scip_decode(const char data[], int size);


#ifdef __cplusplus
}
#endif

#endif /* !URG_SENSOR_H */
