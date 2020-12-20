#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int main ( int argc, char* argv[] ) {
     if ( argc != 2 ) {
          fprintf ( stderr, "Usage: %s <true or false>\n", argv [ 0 ] );
          exit ( -1 );
     }

     bool demonstrate_bug;
     switch ( argv [ 1 ] [ 0 ] ) {
     case 't':
     case 'T':
          demonstrate_bug = true;
          break;
     case 'f':
     case 'F':
          demonstrate_bug = false;
          break;
     default:
          fprintf ( stderr, "Usage: %s <true or false>\n", argv [ 0 ] );
          exit ( -1 );
     }

     char* filename = "/tmp/test_file";

     int fd_write = open ( filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IROTH | S_IRGRP );
     if ( fd_write < 0 ) {
          fprintf ( stderr, "open(write, %s) failed\n", filename );
          exit ( -1 );
     }

     FILE* p_file_write = fopen(filename, "w+");
     if ( p_file_write == NULL ) {
          fprintf ( stderr, "fopen(write, %s) failed\n", filename );
          exit ( -1 );
     }

     FILE* p_file_read = fopen(filename, "r");
     if ( p_file_read == NULL ) {
          fprintf ( stderr, "fopen(read, %s) failed\n", filename );
          exit ( -1 );
     }

     /* Write one chunk of data to the file */
     char* data1 = "HELLO ";
     ssize_t n_written = fwrite ( data1, 1, strlen ( data1 ), p_file_write );
     if ( n_written != strlen ( data1 ) ) {
          fprintf ( stderr, "Unable to write the first chunk of data to %s\n", filename );
          exit ( -1 );
     }
     fflush(p_file_write);

     /* Read the entire chunk of data from the file */
     char read_buffer [ 50 ] = { };
     char* p_read = &read_buffer [ 0 ];
     ssize_t n_read = fread ( p_read, 1, sizeof(data1), p_file_read );
     p_read += n_read;
     printf ( "Reading - n_read is %zd data is \"%s\" feof is %s\n",
              n_read, read_buffer, feof ( p_file_read ) ? "true" : "false" );

     /* Write a second chunk of data to the file */
     char* data2 = "WORLD";
     n_written = fwrite ( data2, 1, strlen ( data2 ), p_file_write );
     if ( n_written != strlen ( data2 ) ) {
          fprintf ( stderr, "Unable to write the second chunk of data to %s\n", filename );
          exit ( -1 );
     }
     fflush(p_file_write);

     /* To avoid the bug which this code was made to demonstrate, we simply
      * need to clear the end of file flag with clearerr before we try a second read */
     if ( !demonstrate_bug ) {
          if ( feof ( p_file_read ) ) {
               clearerr ( p_file_read );
          }
     }

     /* Try to read the second chunk from the file. */
     n_read = fread ( p_read, 1, sizeof(data2), p_file_read );
     printf ( "Reading - n_read is %zd data is \"%s\" feof is %s\n",
              n_read, read_buffer, feof ( p_file_read ) ? "true" : "false" );

     int rc = fclose(p_file_read);
     if ( rc ) {
          fprintf ( stderr, "fclose(read, %s) failed\n", filename );
          exit ( -1 );
     }

     rc = fclose(p_file_write);
     if ( rc ) {
          fprintf ( stderr, "fclose(write, %s) failed\n", filename );
          exit ( -1 );
     }
}
