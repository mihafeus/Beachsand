
//----------------------Transmit code-------------------------------------------

void transmitFunc(String SendStr){

  int str_len = SendStr.length() + 1;                  // Length (with one extra character for the null terminator) 
  char char_array[str_len];                            // Prepare the character array (the buffer)   
  SendStr.toCharArray(char_array, str_len);            // Copy it over
  vw_send((uint8_t *)char_array, strlen(char_array));
  vw_wait_tx();                                        // Wait until the whole message is gone

}
