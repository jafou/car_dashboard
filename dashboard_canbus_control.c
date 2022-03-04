#include <CAN.h>	//used lib: CANduino
#include <SPI.h>

#define BUS_SPEED 500
#define warning_address 0x5F4
#define speed_address 0x5F0

// G E A R :
//                 sumbolo =  8-bit   = decimal
//      1              	A  = 11111100 = 252
//      |               	r  = 00011000 = 24
//      00              	t  = 01011010 = 90
// 2 _ 0  0 _ 3       	1  = 00100100 = 36
//     0  0             	2  = 10111010 = 186 
//      00 - 4          	3  = 10110110 = 182
// 5 _ 0  0 _ 6        4  = 01110100 = 116
//     0  0             	5  = 11010110 = 214
//      00              	6  = 01011110 = 94
//      |               	0  = 11101110 = 238
//      7


int char_A = 252 ;	
int char_r = 24 ;
int char_t = 90 ;
int char_1 = 36 ;
int char_4 = 116 ;

int char_2 = 186;		//93;
int char_3 = 182;		//91;
int char_5 = 214;		//107;
int char_6 = 94;			//111;

long gearLeds = 0;		
long rpmLeds = 1;
int flag1 = 0;			// metabliti pou xrisimopoieitai mono sto arxiko motibo

int data = 5; 			// where we send the bits to control outputs ,ta data pou tha parei o shift register gia ta rpmLeds
int latch = 6;			// tells the shift register when to activate the output sequence
int clock = 7; 		// keeps the data in sync
int enable = 9;		// gia na rythmisw fwteinotita me pwm		//*  *d htane10

int g_data = 4;
int g_latch = 8;
// xrisimopoiw to idio clock kai to idio enable kai sto gear kai sto rpm
int g_clock = clock;
int g_enable = enable;

//int brightness_rpmLeds ; 		// paei gia sbhsimo
//int brightness_gearLeds ; 	//*d paei gia sbhsimo
int brightness ;						// metabliti pou tha pairnei timi apo to potentiometro kai tha antistoixizetai se fwteinothta twn led

unsigned int water_temp_led = 17;			
unsigned int oil_pressure_led = 18;
unsigned int oil_temp_led = 19;

void setup()
{
	Serial.begin(9600);
	// initialize CAN bus class
	// this class initializes SPI communications with MCP2515

	
	pinMode(data,OUTPUT);
	pinMode(clock,OUTPUT);
	pinMode(latch,OUTPUT);
	pinMode(enable,OUTPUT);
	
	pinMode(g_data,OUTPUT);
	pinMode(g_clock,OUTPUT);
	pinMode(g_latch,OUTPUT);
	pinMode(g_enable,OUTPUT);
	
	pinMode(water_temp_led, OUTPUT);
	pinMode(oil_temp_led, OUTPUT);
	pinMode(oil_pressure_led, OUTPUT);
	
	
	
	
	for (int i = 0; i < 50; i++){
				
		if (i < 10){						//A
		gearLeds = char_A;}
		
		if (i >= 10 && i <19){		//r
		gearLeds = char_r;}
		
		if (i >= 19 && i <29){		//t
		gearLeds = char_t;}
		
		if (i >= 29 && i <38){		//1
		gearLeds = char_1;}
		
		if (i >= 38 && i <47){		//4
		gearLeds = char_4;}
		
		if (flag1 == 0) {
			rpmLeds = rpmLeds * 2;
			if (rpmLeds > 8191) {			//4095 = Β 0000 1111 1111 1111 ;; 8191 = Β 0001 1111 1111 1111
				flag1 = 1;
			}
		}
		else if (flag1 == 1){
			rpmLeds = rpmLeds / 2;
			if (rpmLeds == 1){
				flag1 = 2;
			}
		}	
		else if ( flag1 == 2) {
			rpmLeds = rpmLeds*2 + 1;
			if (rpmLeds > 4096){
				flag1 = 3;
			}
		}
		else if (flag1 == 3) {
			rpmLeds = rpmLeds / 2;
		}
				
		brightness = map(analogRead(1),0,797,255,0);
		//brightness = map(analogRead(1),0,797,255,0);
		Serial.print("brightness :");
		Serial.println(brightness);
	
		//analogRead(1);
		digitalWrite(latch, LOW);
		
		analogWrite(enable, brightness); 						//*d
		shiftOut(data, clock, MSBFIRST, (rpmLeds >> 8));		// shiftOut HIGHBYTE
		shiftOut(data, clock, MSBFIRST, rpmLeds);				// shiftOut LOWBYTE
		
		digitalWrite(latch, HIGH);
		
		
		digitalWrite(g_latch, LOW);
		shiftOut(g_data, g_clock, LSBFIRST, gearLeds);
		digitalWrite(g_latch, HIGH);
		analogWrite(g_enable, brightness);
		
		delay(100);
	}
	
	for (int i=1; i<6;i++)
	{
		if (i%2)
		{
			gearLeds = 8191;
			rpmLeds = 8191;
			digitalWrite(latch, LOW);
		
			analogWrite(enable, brightness); 						//*d
			shiftOut(data, clock, MSBFIRST, (rpmLeds >> 8));		// shiftOut HIGHBYTE
			shiftOut(data, clock, MSBFIRST, rpmLeds);				// shiftOut LOWBYTE
			
			digitalWrite(latch, HIGH);
			
			
			digitalWrite(g_latch, LOW);
			shiftOut(g_data, g_clock, MSBFIRST, gearLeds);
			digitalWrite(g_latch, HIGH);
			analogWrite(enable, brightness);
		}
		else
		{
			gearLeds = 0;
			rpmLeds = 0;
			digitalWrite(latch, LOW);
			
			shiftOut(data, clock, MSBFIRST, (rpmLeds >> 8));		// shiftOut HIGHBYTE
			shiftOut(data, clock, MSBFIRST, rpmLeds);				// shiftOut LOWBYTE
			
			digitalWrite(latch, HIGH);
			analogWrite(enable, brightness); 
			
			digitalWrite(g_latch, LOW);
			shiftOut(g_data, g_clock, MSBFIRST, gearLeds);
			digitalWrite(g_latch, HIGH);
			analogWrite(enable, brightness);
		}
		delay(100);
	}
	
	CAN.begin();
	CAN.baudConfig(BUS_SPEED);
	CAN.setMode(LISTEN);  
	delay(10) ;
	
}	

/*	
	100000000000 1	2048	
	110000000000 2	3072
	111000000000 3	3584
	111100000000 4	3840
	111110000000 5	3968
	111111000000 6	4032
	111111100000 7	4064
	111111110000 8	4080
	111111111000 9	4088
	111111111100 10	4092
	111111111110 11	4094
	111111111111 12	4095
*/

// Όρια για αλλαγή κατάστασης των RPM
unsigned int thres1 = 2500;				// 1 led *mexri tis thres1 anabei to 1 led
unsigned int thres2 = 6000;				// 2 led  *mexri tis thres2 anaboun ta 2 led k.o.k.
unsigned int thres3 = 7000;				// 3 led
unsigned int thres4 = 8000;				// 4 led
unsigned int thres5 = 9000;				// 5 led
unsigned int thres6 = 10000;				// 6 led
unsigned int thres7 = 10500;				// 7 led
unsigned int thres8 = 11000;				// 8 led
unsigned int thres9 = 11500;				// 9 led
unsigned int thres10 = 12000;				// 10 led
unsigned int thres11 = 13000;				// 11 led
unsigned int thres12 = 14000;				// 12 led

unsigned int ideal_rpm = 9500;							// strofes idanikes gia allagh taxythtas -> tha anabosbhnei to kantran
unsigned int max_rpm = 13500;							// strofes pou de prepei na exw tha sbhnoun ta led

unsigned int ideal_rpm_1 = 9500;
unsigned int ideal_rpm_2 = 12200;
unsigned int ideal_rpm_3 = 12100;
unsigned int ideal_rpm_4 = 12000;
unsigned int ideal_rpm_5 = 12000;

int i;							// h metablhth ayth upodeknyei th sxesh pou exw sto kibwtio
int i1;						// i1 - i 6 : counters poy metrane poses fores apo tis 20 epanalipseis eixa 1,2,3.. sxesi
int i2;
int i3;
int i4;
int i5;
int i6;
int iguros;					// o counter pou tha metrisei poses fores tha ginei h loop() gia na dw poia sxesh exw perissoteres fores

unsigned int water_temp;			//@	can bus
unsigned int oil_pressure;			//@
unsigned int oil_temp;				//@


unsigned int division;		// "sigmiaia" diairesh : wheelspeed / rpm   (x1000)
unsigned int division_;		// o m.o. twn diairesewn -> tha ay3hsei to i1,i2 klp
unsigned int divisionSum;

unsigned int rpm;			//@
unsigned int rpm_;
unsigned int rpmSum;

unsigned int wheelSpeed;	//@
unsigned int wheelSpeed_;

unsigned int speedSum;

int loop_counter = 0;
int counter_gear = 0;
const int counter_gear_mult = 5;

void extract_vars(byte * frame, unsigned int &a, unsigned int &b, unsigned int &c, unsigned int &d){
	a = (frame[0] << 8) + frame[1] ;
	b = (frame[2] << 8) + frame[3] ;
	c = (frame[4] << 8) + frame[5] ;
	d = (frame[6] << 8) + frame[7] ;
}

void loop()
{

	  //*****************************************************
	  //****************rythmiseis gia to can********************
	{//*****************************************************
	byte length,rx_status;
	unsigned short frame_id;
	byte frame_data[8];
	unsigned int tempo;

	//clear receive buffers
	frame_data[0] = 0x00;
	frame_data[1] = 0x00;
	frame_data[2] = 0x00;
	frame_data[3] = 0x00;
	frame_data[4] = 0x00;
	frame_data[5] = 0x00;
	frame_data[6] = 0x00;
	frame_data[7] = 0x00;

	frame_id = 0x0000;
	length = 0;	

	rx_status = CAN.readStatus();
	
	//read buffers
	if ((rx_status & 0x01) == 0x01){   
	CAN.readDATA_ff_0(&length,frame_data,&frame_id);	
	} 
	else if ((rx_status & 0x02) == 0x02) {	
	CAN.readDATA_ff_1(&length,frame_data,&frame_id);   
	}
	
	//extract variables from CAN frame
	if (((rx_status & 0x01) == 0x01) ||
	  ((rx_status & 0x02) == 0x02)) {  
	  
		//warnings
		if(frame_id == warning_address){
			extract_vars(frame_data, water_temp, oil_pressure,  tempo, tempo ) ;
		}
		
		//speeds
		if(frame_id == speed_address){
			//extract_vars(frame_data, _rpm, _wheelSpeed, tempo, tempo) ;
			extract_vars(frame_data, rpm, wheelSpeed, tempo, tempo) ;
		}			
	}
	
	}
	
	//rpm = (unsigned int)random(1000,14000);
	//rpm = 11500;
	showRPM(rpm);
	delay(10);
	Serial.println(rpm);
	// έχω εμφανίσει την τιμή των rpm και τώρα θα υπολογίσω και τη τιμή των gearLeds
	
	
	
	
	
	
	
	//********************************************************************
	//***********************AMAKSI AKINITO*******************************
	//********************************************************************
	
	if (wheelSpeed <= 5)
	{
		//showRPM(rpm)
		gearLeds = 238 ; 	// gear = 0 (neutral)
		
		iguros=0;
		i1=0;
		i2=0;
		i3=0;
		i4=0;
		i5=0;
		i6=0;	
	}
	
	//********************************************************************
	//***********************AMAKSI KINEITAI******************************
	//********************************************************************
	
	
	
	if (wheelSpeed > 5)
	{
		float ig = ( rpm * 1.208 ) / ( 2.111 * wheelSpeed );
		
		
		/*
		for (int i = 0; i<20; i++)
		{
			float ig = ( rpm * 1.208 ) / ( 2.111 * wheelSpeed );
			//sum_ig = sum_ig + ig;
		}
		*/
		
		
		
		if ( (ig > 2.383) && (ig <= 2.783) )
		{
			gearLeds = char_1;								//antistoixei sti 1h	2.583 +- 0.2
			ideal_rpm = ideal_rpm_1;
		}
		if ( (ig > 1.85) && (ig <= 2.15) )
		{
			gearLeds = char_2;								//antistoixei sti 2h	2.000 +- 0.15
			ideal_rpm = ideal_rpm_2;
		}
		if ( (ig > 1.567) && (ig <= 1.767) )
		{
			gearLeds = char_3;								//antistoixei sti 3h	1.667 +- 0.1
			ideal_rpm = ideal_rpm_3;
		}
		if ( (ig > 1.394) && (ig <= 1.494) )
		{
			gearLeds = char_4;								//antistoixei sti 4h	1.444 +- 0.05
			ideal_rpm = ideal_rpm_4;
		}
		if ( (ig > 1.254) && (ig <= 1.354) )
		{
			gearLeds = char_5;								//antistoixei sti 5h	1.304 +- 0.05
			ideal_rpm = ideal_rpm_5;
		}
		if ( (ig > 1.158) && (ig <= 1.253) )
		{
			gearLeds = char_6;								//antistoixei sti 6h	1.208 +- 0.05
			//ideal_rpm = ideal_rpm_6;
		}
		
		
		
		
	}
	
		
	showGEAR(gearLeds);
	
	
	
	 // *********************************************************************
	 // *************************ENDEIKSEIS SFALMATWN***********************
	{// *********************************************************************
	
	
	// to parakatw allaxthke 16/7/2014
	
	if (oil_pressure > 50)
		{
		digitalWrite(oil_pressure_led, HIGH);
		showGEAR(218);
		delay(50);
		showGEAR(0);
		delay(50);
		}
	else
		{
		digitalWrite(oil_pressure_led, LOW);
		}
	
	/*
	if (oil_temp == 1)
		{
		digitalWrite(oil_temp_led, HIGH);
		}
	else
		{
		digitalWrite(oil_temp_led, LOW);
		}	
	*/
	
	// to parapanw allaxthke 16/7/2014
	if (water_temp > 70)
		{
		digitalWrite(water_temp_led, HIGH);
		showGEAR(218);
		delay(100);
		showGEAR(0);
		delay(100);
		
		
		//anabosbhnw sto gear indicator to gramma E - error me argo rythmo sxetika gia na katalabainei o odhgos
		}
	else
		{
		digitalWrite(water_temp_led, LOW);
		}
	}

	
}


/*	 Αντιστοίχηση στροφών σε led
	100000000000 1	2048		-> 0000 0000 0001  1
	110000000000 2	3072    	-> 0000 0000 0011  3
	111000000000 3	3584    	-> 0000 0000 0111  7
	111100000000 4	3840    	-> 0000 0000 1111  15
	111110000000 5	3968    	-> 0000 0001 1111  31
	111111000000 6	4032    	-> 0000 0011 1111  63
	111111100000 7	4064    	-> 0000 0111 1111  127
	111111110000 8	4080    	-> 0000 1111 1111  255
	111111111000 9	4088    	-> 0001 1111 1111  511
	111111111100 10	4092    	-> 0011 1111 1111  1023
	111111111110 11	4094    	-> 0111 1111 1111  2047
	111111111111 12	4095    	-> 1111 1111 1111  4095
*/
void showRPM(unsigned int Rpm)
{
	if ( Rpm < thres1 ){
		rpmLeds = 1;
		}
	else if ( Rpm < thres2 ){
		rpmLeds = 3;
		}
	else if ( Rpm < thres3 ){
		rpmLeds = 7; 
		}
	else if ( Rpm < thres4 ){
		rpmLeds = 15;
		}	
	else if ( Rpm < thres5 ){
		rpmLeds = 31;
		}	
	else if ( Rpm < thres6 ){
		rpmLeds = 63;
		}
	else if ( Rpm < thres7 ){
		rpmLeds = 127; 
		}	
	else if ( Rpm < thres8 ){
		rpmLeds = 255;
		}	
	else if ( Rpm < thres9 ){
		rpmLeds = 511; 
		}	
	else if ( Rpm < thres10 ){
		rpmLeds = 1023;
		}	
	else if ( Rpm < thres11 ){
		rpmLeds = 2047;
		}	
	else if ( Rpm < thres12 ){
		rpmLeds = 4095;
		}
	

	if ( Rpm >= max_rpm ) 
		{
			rpmLeds = 0;
		}

	//εμφάνιση τιμής των rpm στα led.
	//to A0 pin einai to kritirio gia th fwteinothta tou strofometrou
	//to A1 pin einai to kritirio gia th fwteinothta tou deikti taxythtas
	//brightness_rpmLeds = map(analogRead(0),0,500,255,0);
	//brightness_gearLeds = map(analogRead(1),0,500,255,0);

	
	
	if ( (Rpm > ( ideal_rpm - 200 )) && (Rpm < ( ideal_rpm + 100 )) )
	{
			brightness = map(analogRead(0),0,500,255,128);
			
			int temp_half = 4032; 
			digitalWrite(latch, LOW);
			shiftOut(data, clock, MSBFIRST, temp_half >>8);			// shiftOut HIGHBYTE
			shiftOut(data, clock, MSBFIRST, temp_half);					// shiftOut LOWBYTE
			digitalWrite(latch, HIGH);
			analogWrite(enable, brightness);
			
			delay(50);
			
			temp_half = 0;
			digitalWrite(latch, LOW);
			shiftOut(data, clock, MSBFIRST, (temp_half >> 8));		// shiftOut HIGHBYTE
			shiftOut(data, clock, MSBFIRST, temp_half);					// shiftOut LOWBYTE
			digitalWrite(latch, HIGH);
			analogWrite(enable, brightness);
			
			delay(50);
			
			// to parakatw prostethike 16/7/2014
			temp_half = 4032; 
			digitalWrite(latch, LOW);
			shiftOut(data, clock, MSBFIRST, temp_half >>8);			// shiftOut HIGHBYTE
			shiftOut(data, clock, MSBFIRST, temp_half);					// shiftOut LOWBYTE
			digitalWrite(latch, HIGH);
			analogWrite(enable, brightness);
			
			
			delay(50);
			
			temp_half = 0;
			digitalWrite(latch, LOW);
			shiftOut(data, clock, MSBFIRST, (temp_half >> 8));		// shiftOut HIGHBYTE
			shiftOut(data, clock, MSBFIRST, temp_half);					// shiftOut LOWBYTE
			digitalWrite(latch, HIGH);
			analogWrite(enable, brightness);
			
			delay(50);
			// to parakanw prostethike 16/7/2014
	}
	else
	{
		brightness = map(analogRead(0),0,500,255,128);

		digitalWrite(latch, LOW);
		shiftOut(data, clock, MSBFIRST, (rpmLeds >> 8));			// shiftOut HIGHBYTE
		shiftOut(data, clock, MSBFIRST, rpmLeds);						// shiftOut LOWBYTE
		digitalWrite(latch, HIGH);
		analogWrite(enable, brightness);
		}
	
	
	
	Serial.println(rpmLeds);
	//delay(1);						
}

void showGEAR(int gLeds)
{
	brightness = map(analogRead(0),0,500,255,128);

	digitalWrite(g_latch, LOW);
	shiftOut(g_data, g_clock, LSBFIRST, gLeds);
	digitalWrite(g_latch, HIGH);
	analogWrite(g_enable, brightness);
	
	//delay(1);
}