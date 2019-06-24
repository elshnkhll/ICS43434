unsigned short int numOfBlanks = 500, smoothingReads = 3;

i2s_config_t i2s_config = {
mode: (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
  sample_rate: 8000 * smoothingReads,
bits_per_sample: I2S_BITS_PER_SAMPLE_32BIT,
channel_format: I2S_CHANNEL_FMT_ONLY_RIGHT,
communication_format: (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_LSB),
intr_alloc_flags: ESP_INTR_FLAG_LEVEL1,
  dma_buf_count: 8,
  dma_buf_len: 8
};

i2s_pin_config_t pin_config = {
  .bck_io_num   = 32,                // BCK  (SCK)    - bit clock or serial clock
  .ws_io_num    = 33,                // LRCK (WS, FS) - left-right clock or word select or frame sync
  .data_out_num = I2S_PIN_NO_CHANGE, // DATA output   - not used
  .data_in_num  = 27                 // DIN  (SD)     - serial data in  (SDATA, SDIN, SDOUT, DACDAT, ADCDAT)
};

const int i2s_num = 0;
int retStat = 0;


uint32_t sampleIn = 0;


int32_t avgGain = 0, peaks[2] = {0, 0}, avgSampleIn = 0;


void Mic_Init() {
  pinMode(27, INPUT);
  //Set up pin 21 and 25 as the BCK and LRCK pins
  pinMode(32, OUTPUT);
  pinMode(33, OUTPUT);
  //Init the i2s device
  i2s_driver_install((i2s_port_t)i2s_num, &i2s_config, 0, NULL);
  i2s_set_pin((i2s_port_t)i2s_num, &pin_config);
  i2s_start((i2s_port_t)i2s_num);

  Serial.print("\r\nBegin Level detect...");
  Serial.print("\r\n\tRead 4000 samples to level out...");
  //This pulls in a bunch of samples and does nothing, its just used to settle the mics output
  for (retStat = 0; retStat < numOfBlanks * 2; retStat++)  {
    i2s_pop_sample((i2s_port_t)i2s_num, (char*)&sampleIn, portMAX_DELAY);
    delay(1);
  }
  //Pull in x number of samples (IN A QUITE ENVIRONMENT) and create the base gain average(the zero point for the mic)
  Serial.print("\r\n\tRead 2000 samples to get avg...");
  for (retStat = 0; retStat < numOfBlanks; retStat++)  {
    i2s_pop_sample((i2s_port_t)i2s_num, (char*)&sampleIn, portMAX_DELAY);
    sampleIn >>= 14;
    avgGain -= sampleIn;
    delay(1);
  }
  avgGain = avgGain / numOfBlanks;
  Serial.printf("\t\tAVG Gain=\t%i", avgGain);
  peaks[0] = avgGain;
  peaks[1] = -avgGain;
  Serial.printf("\r\n\tSetting MAX gain to\t%i\tMin Gain to\t%i\r\n", peaks[0], peaks[1]);
  delay(1000);
}

int32_t Mic_Get_Sample() {
  sampleIn = 0;
  avgSampleIn = 0;
  int16_t sampleIn16 = 0;
  //read in smoothingReads number of times and use the average
  for (retStat = 0; retStat < smoothingReads; retStat++)  {
    i2s_pop_sample((i2s_port_t)i2s_num, (char*)&sampleIn, portMAX_DELAY);
    sampleIn >>= 8;
    sampleIn16 = sampleIn;
    avgSampleIn += sampleIn16;
  }

  sampleIn16 = round((float)avgSampleIn / smoothingReads);

  avgSampleIn = round( ((float)(sampleIn16 + 40000) / 40000 ) * 16000 ); // 16 bit unsigned

  return avgSampleIn;

}
