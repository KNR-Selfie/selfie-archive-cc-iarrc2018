#ifndef URG_RING_BUFFER_H
#define URG_RING_BUFFER_H

/*!
  \file
  \brief Ring buffer functions
  \author Satofumi KAMIMURA

  $Id$
*/


//!  Control structure of the ring buffer
typedef struct
{
    char *buffer;                 //!<  Pointer to the data buffer
    int buffer_size;              //!<  Buffer size
    int first;                    //!<  Index of the first entry of the buffer
    int last;                     //!<  Index of the last entry of the buffer
} ring_buffer_t;


/*!
  \brief Initialization

  \param[in] ring Pointer to the ring buffer data structure
  \param[in] buffer Actual buffer to use
  \param[in] shift_length Buffer size as multiple of 2
*/
extern void ring_initialize(ring_buffer_t *ring,
                            char *buffer, const int shift_length);


/*!
  \brief Clears the ring  buffer

  \param[in] ring Pointer to the ring buffer data structure
*/
extern void ring_clear(ring_buffer_t *ring);


/*!
  \brief Returns the number of elements on the buffer

  \param[in] ring Pointer to the ring buffer data structure
*/
extern int ring_size(const ring_buffer_t *ring);


/*!
  \brief Returns the maximum number of elements of the ring buffer

  \param[in] ring Pointer to the ring buffer data structure
*/
extern int ring_capacity(const ring_buffer_t *ring);


/*!
  \brief Stores data on the ring buffer

  \param[in] ring Pointer to the ring buffer data structure
  \param[in] data Data to store
  \param[in] size Number of elements to store

  \return The number of elements written to the ring buffer
*/
extern int ring_write(ring_buffer_t *ring, const char *data, int size);


/*!
  \brief Extracts data from the ring buffer

  \param[in] ring Pointer to the ring buffer data structure
  \param[out] buffer Buffer to hold the extracted data
  \param[in] size Maximum size of the buffer

  \return The number of elements read from the ring buffer
*/
extern int ring_read(ring_buffer_t *ring, char *buffer, int size);

#endif /* ! RING_BUFFER_H */
