//#######################################################################################################
//##                    This Plugin is only for use with the RFLink software package                   ##
//##                                     Plugin-083: Brel Motor                                            ##
//#######################################################################################################
/*********************************************************************************************\
 * This plugin takes care of decoding and encoding the Brel Motor / Dooya protocol (DC318/DC114)
 * 
 * Author  (present)  : Sebastien LANGE
 * Support (present)  : https://github.com/couin3/RFLink 
 * Author  (original) : Sebastien LANGE
 * Support (original) : https://github.com/couin3/RFLink 
 * License            : This code is free for use in any open source project when this header is included.
 *                      Usage of any parts of this code in a commercial application is prohibited!
 *********************************************************************************************
 * Changelog: v1.0 initial release
 *********************************************************************************************
 * Technical Information:
 * Decodes signals from a DOOYA receiver
 * DOOYA protocol
 * 1101 0110 1000 0000 1101 1111 1111 0000
 * AAAA AAAA AAAA AAAA AAAA AAAA BBBB CCCC
 *
 * A = Remote ID
 * B = Remote channel
 * C = Command (UP/STOP/DOWN/RESET)
 * 
 * Sample:
 * 20;XX;DEBUG;Pulses=82;Pulses(uSec)=4640,1504,192,640,192,640,512,320,192,608,512,320,192,640,480,320,480,352,160,640,512,320,192,640,160,640,192,640,160,640,160,640,192,640,512,320,480,320,160,640,160,640,480,320,160,640,160,640,160,672,160,640,192,640,192,640,192,640,192,640,192,640,192,640,512,352,160,640,160,640,160,640,512,320,512,320,512,320,480,320,160,4992;
 \*********************************************************************************************/
#define DOOYA_PLUGIN_ID 083
#define PLUGIN_DESC_083 PSTR("DOOYA")
#define DOOYA_PULSECOUNT_1 82
// #define DOOYA_PULSECOUNT_2 54
// #define DOOYA_PULSECOUNT_3 60
// #define DOOYA_PULSECOUNT_4 62
// #define DOOYA_PULSECOUNT_5 68
// #define DOOYA_PULSECOUNT_6 72
// #define DOOYA_PULSECOUNT_6 84

#define DOOYA_MIDVALUE 384 / RAWSIGNAL_SAMPLE_RATE

#define DOOYA_PULSEMIN 1600 / RAWSIGNAL_SAMPLE_RATE
#define DOOYA_PULSEMINMAX 2200 / RAWSIGNAL_SAMPLE_RATE
#define DOOYA_PULSEMAXMIN 3000 / RAWSIGNAL_SAMPLE_RATE

#define DOOYA_NEAR_SYNC_START 36000
#define DOOYA_EXACT_SYNC_END_1   0x1380
// #define DOOYA_EXACT_SYNC_END_2   0x079C
// #define DOOYA_EXACT_SYNC_END_3   0x119C
// #define DOOYA_EXACT_SYNC_END_4   0x139C
// #define DOOYA_EXACT_SYNC_END_5   0x129C
// #define DOOYA_EXACT_SYNC_END_6   0x069C
// #define DOOYA_EXACT_SYNC_END_7   0x109C
// #define DOOYA_EXACT_SYNC_END_8   0x0e9C

#define DOOYA_UP_COMMAND    0x11 // 0001 0001
#define DOOYA_STOP_COMMAND  0x55 // 0101 0101
#define DOOYA_DOWN_COMMAND  0x33 // 0011 0011
#define DOOYA_SETUP_COMMAND 0xcc // 1100 1100


#ifdef PLUGIN_083
#include "../4_Display.h"

boolean Plugin_083(byte function, char *string)
{
   // if ((RawSignal.Number != DOOYA_PULSECOUNT_1)
   //    && (RawSignal.Number != DOOYA_PULSECOUNT_2)
   //    && (RawSignal.Number != DOOYA_PULSECOUNT_3)
   //    && (RawSignal.Number != DOOYA_PULSECOUNT_4)
   //    && (RawSignal.Number != DOOYA_PULSECOUNT_5)
   //    && (RawSignal.Number != DOOYA_PULSECOUNT_6))
   //    return false;

   int i;

   int last_byte = RawSignal.Pulses[RawSignal.Number-1]*0x100 + RawSignal.Pulses[RawSignal.Number];
   char dbuffer[64];

      if (RawSignal.Number == DOOYA_PULSECOUNT_1 
         // && last_byte  == DOOYA_EXACT_SYNC_END_1
         ) 
      {
         byte bbuffer[128];

         sprintf_P(dbuffer, PSTR("Pulses %04d Multiply %04d Delay %04d : "), 
            RawSignal.Number, RawSignal.Multiply, RawSignal.Delay);
         Serial.print(dbuffer);

         int j;
         int buffer_index=0;

         // // Skip start and end sequence
         for (j = 3; j < RawSignal.Number-1; j+=8)
         {
            byte short_bitstream=0;
   
            for (i = 0; i < 8; i+=2)
            {
               short_bitstream <<= 1; // Always shift

               // if (RawSignal.Pulses[j+i] < RawSignal.Pulses[j+i+1]) 
               if (RawSignal.Pulses[j+i] < DOOYA_MIDVALUE) 
               {
                  short_bitstream |= 0x0; // long bit = 0
               }
               else
               {
                  short_bitstream |= 0x1; // long bit = 1
               }
            }

            bbuffer[buffer_index++]=short_bitstream;
            sprintf(dbuffer, "0x%x ", short_bitstream);
            Serial.print(dbuffer);
         }
         sprintf_P(dbuffer, PSTR("END %02x%02x"), RawSignal.Pulses[RawSignal.Number-1], RawSignal.Pulses[RawSignal.Number]);
         Serial.print(dbuffer);

         Serial.print(F(";\r\n"));
         /**
          * bbuffer :
          * [0-5] : ID
          * [6]
          * [7] : Channel
          * [8-9] : COMMAND
          **/

         long address = bbuffer[0]*0x100000
                        +bbuffer[1]*0x10000
                        +bbuffer[2]*0x1000
                        +bbuffer[3]*0x100
                        +bbuffer[4]*0x10
                        +bbuffer[5];

         display_Header();
         display_Name(PSTR("BrelMotor"));
         display_IDn(address, 6);
         display_SWITCH(bbuffer[7]);

         int command = bbuffer[8]*0x10+bbuffer[9];

         switch (command)
         {
         case DOOYA_UP_COMMAND:
            display_Name(PSTR("CMD=UP"));
            break;
         case DOOYA_DOWN_COMMAND:
            display_Name(PSTR("CMD=DOWN"));
            break;
         case DOOYA_STOP_COMMAND:
            display_Name(PSTR("CMD=STOP"));
            break;
         case DOOYA_SETUP_COMMAND:
            display_Name(PSTR("CMD=SETUP"));
            break;
         } 
         display_Footer();
      } 
      else 
      {
         sprintf_P(dbuffer, PSTR("Pulses %04d END %02x%02x"), RawSignal.Number, RawSignal.Pulses[RawSignal.Number-1], RawSignal.Pulses[RawSignal.Number]);
         Serial.println(dbuffer);
         return false;
      }  

   RawSignal.Repeats = true; // suppress repeats of the same RF packet
   RawSignal.Number = 0;
   return true;
}
#endif // PLUGIN_0083

#ifdef PLUGIN_TX_083
#include "../1_Radio.h"
#include "../2_Signal.h"
#include "../3_Serial.h"
#include <stdlib.h>

#define DOOYA_RFSTART_0 4512
#define DOOYA_RFSTART_1 1472
#define DOOYA_RFEND_0 4992
#define DOOYA_RFLOW   192
#define DOOYA_RFHIGH  608

// void addLowHighSignalPulses(unsigned long low, unsigned long high, 
//    unsigned long value, int size, int *currrentPulses)  {
//    for (byte x = (*currrentPulses + size*2); x >= *currrentPulses; x = x - 2)
//    {
//       if ((value & 1) == 1)
//       {
//          RawSignal.Pulses[x] = low / RawSignal.Multiply;
//          RawSignal.Pulses[x - 1] = high / RawSignal.Multiply;
//       }
//       else
//       {
//          RawSignal.Pulses[x] = high / RawSignal.Multiply;
//          RawSignal.Pulses[x - 1] = low / RawSignal.Multiply;
//       }
//       value = value >> 1;
//    }
//    *currrentPulses+=size*2;
// }

byte nibble2c(char c)
{
   if ((c>='0') && (c<='9'))
      return c-'0' ;
   if ((c>='A') && (c<='F'))
      return c+10-'A' ;
   if ((c>='a') && (c<='f'))
      return c+10-'a' ;
   return -1 ;
}

void addLowHighSignalPulses(unsigned long low, unsigned long high, 
   char * hexvalue, int size, int *currrentPulses)  {
   // for (byte x = *currrentPulses; x <= (*currrentPulses + size*2); x = x + 2)
   int i=0;
   while (*(hexvalue+i) != '\0') 
   {
      Serial.print(*(hexvalue+i));
      Serial.print("(");

      byte value = nibble2c(*(hexvalue+i));
      for (int x=0; x < 4; x++)
      {
         if ((value & B1000) == B1000)
         {
            Serial.print("1");
            RawSignal.Pulses[(*currrentPulses)++] = high / RawSignal.Multiply;
            RawSignal.Pulses[(*currrentPulses)++] = low / RawSignal.Multiply;
         }
         else
         {
            Serial.print("0");
            RawSignal.Pulses[(*currrentPulses)++] = low / RawSignal.Multiply;
            RawSignal.Pulses[(*currrentPulses)++] = high / RawSignal.Multiply;
         }
         value = value << 1;
      }
      Serial.print(")");
      i++;
   }
}

void debugRawSignal(int size) 
{
   char dbuffer[64];

   sprintf_P(dbuffer, PSTR("Pulses %04d Multiply %04d: "), 
            RawSignal.Number, RawSignal.Multiply, RawSignal.Time);
   Serial.print(dbuffer);

   for (int i=0; i<size; i++) 
   {
      sprintf_P(dbuffer, PSTR("%d "), RawSignal.Pulses[i]*RawSignal.Multiply);
      Serial.print(dbuffer);
   }

   Serial.println();
}
void sendRF(int currentPulses) 
{
   noInterrupts();
 
   for (int j=0; j < RawSignal.Repeats; j++) {
      for (int i = 0; i < currentPulses; i=i+2) 
      {
      digitalWrite(PIN_RF_TX_DATA, HIGH);
      delayMicroseconds(RawSignal.Pulses[i]*RawSignal.Multiply);
      digitalWrite(PIN_RF_TX_DATA, LOW);
      delayMicroseconds(RawSignal.Pulses[i+1]*RawSignal.Multiply);
      }
   }

   interrupts();
}

void addSinglePulse(unsigned long value, int *currrentPulses)
{
   RawSignal.Pulses[(*currrentPulses)++] = value / RawSignal.Multiply;
}

boolean PluginTX_083(byte function, char *string)
{
   Serial.println(F("PluginTX_083"));

   char dbuffer[30] ;
   boolean success = false;
   unsigned long bitstream = 0L;

   sprintf_P(dbuffer, PSTR("%s"), string);
   Serial.println(dbuffer);

   //20;XX;DEBUG;Pulses=82;Pulses(uSec)=4704,1472,192,608,192,608,512,288,192,608,512,288,192,608,512,288,512,320,192,608,512,288,192,608,192,608,192,608,192,608,192,608,192,640,512,288,512,320,192,640,192,640,512,288,192,640,192,608,192,640,192,640,192,608,192,640,192,640,512,320,512,320,512,320,512,352,160,640,192,640,192,640,512,320,512,320,512,320,512,320,192,4992;

   //20;XX;DEBUG;Pulses=82;Pulses(uSec)=4512,1472,192,608,192,608,512,288,192,608,512,288,192,608,512,288,512,320,192,608,512,288,192,608,192,608,192,608,192,608,192,608,192,640,512,288,512,288,192,608,192,608,512,288,192,608,192,608,192,640,192,608,192,608,192,608,192,608,512,288,512,288,512,288,512,320,192,608,512,288,192,608,512,288,192,608,512,288,192,608,512,4992;

   //10;BrelMotor;2b40c8;01;UP;
   //0123456789012345678901234567890123456789
   //0         10        20        30        40
   //
   // Pulses 0082 Multiply 0032 : 0x2 0xb 0x4 0x0 0xc 0x8 0x0 0xf 0x1 0xe END 069c;

   if (strncasecmp(string + 3, "BrelMotor;", 10) == 0)
   {
   //    // Override with 0x to conform strtoul base 16
   //    string[11]='0';
   //    string[12]='x';
   // sprintf_P(dbuffer, PSTR("%s"), string);
   // Serial.println(dbuffer);

      int command;
      char *strings[10];
      char *ptr = NULL;

      byte index = 0;
      ptr = strtok(string, ";");  // takes a list of delimiters
      while(ptr != NULL)
      {
         strings[index] = ptr;
         index++;
         ptr = strtok(NULL, ";");  // takes a list of delimiters
      }

      char * address = strings[2]; 
      char * subaddress = strings[3];
      char * commandstring = strings[4];

      sprintf_P(dbuffer, PSTR("Send BrelMotor %s %s"), address, subaddress);
      Serial.println(dbuffer);

      if (strncasecmp(commandstring, "on", 2) == 0)
         command = DOOYA_UP_COMMAND;
      else if (strncasecmp(commandstring, "up", 2) == 0)
         command = DOOYA_UP_COMMAND;
      else if (strncasecmp(commandstring, "off", 3) == 0)
         command = DOOYA_DOWN_COMMAND;
      else if (strncasecmp(commandstring, "stop", 4) == 0)
         command = DOOYA_STOP_COMMAND;
      else if (strncasecmp(commandstring, "down", 4) == 0)
         command = DOOYA_DOWN_COMMAND;
      else if (strncasecmp(commandstring, "go_up", 5) == 0)
         command = DOOYA_UP_COMMAND;
      else if (strncasecmp(commandstring, "go_down", 7) == 0)
         command = DOOYA_DOWN_COMMAND;
      if (command == 0)
         return false;

      sprintf_P(dbuffer, PSTR("Send BrelMotor %s %s %s"), address, subaddress, command);
      Serial.println(dbuffer);

      RawSignal.Repeats = 5;
      RawSignal.Multiply = 32;
     // AC_Send(ADDRESS << 2 && SWITCH, command);

      int currentPulses=0;

      // add Headers
      addSinglePulse(DOOYA_RFSTART_0, &currentPulses);
      addSinglePulse(DOOYA_RFSTART_1, &currentPulses);
      // add Body
      addLowHighSignalPulses(DOOYA_RFLOW, DOOYA_RFHIGH, address, 6*8, &currentPulses);
      addLowHighSignalPulses(DOOYA_RFLOW, DOOYA_RFHIGH, subaddress, 2*8, &currentPulses);
      sprintf_P(dbuffer, PSTR("%x"), command);
      addLowHighSignalPulses(DOOYA_RFLOW, DOOYA_RFHIGH, dbuffer, 2*8, &currentPulses);
      // add Footer
      addSinglePulse(0, &currentPulses);
      addSinglePulse(DOOYA_RFEND_0, &currentPulses);

      RawSignal.Number = currentPulses;
      debugRawSignal(currentPulses);

      RawSignal.Multiply = 38;
      sendRF(currentPulses);

      return true;

      //-----------------------------------------------
      RawSignal.Multiply = 50;
      RawSignal.Repeats = 10;
      RawSignal.Delay = 20;
      RawSignal.Pulses[currentPulses++] = DOOYA_RFLOW / RawSignal.Multiply;
      RawSignal.Pulses[currentPulses++] = DOOYA_RFLOW / RawSignal.Multiply;

      // addLowHighSignalPulses(DOOYA_RFLOW, DOOYA_RFHIGH, ADDRESS, &currrentPulses, 6*8);

      // addLowHighSignalPulses(DOOYA_RFLOW, DOOYA_RFHIGH, SWITCH, &currrentPulses, 2*8);

      // addLowHighSignalPulses(DOOYA_RFLOW, DOOYA_RFHIGH, command, &currrentPulses, 2*8);

      RawSignal.Number = currentPulses;

      sprintf_P(dbuffer, PSTR("Pulses %04d END %02x%02x"), RawSignal.Number, RawSignal.Pulses[RawSignal.Number-1], RawSignal.Pulses[RawSignal.Number]);
      Serial.println(dbuffer);

      return true;

      // RawSendRF();
      success = true;
      //-----------------------------------------------
   }
   return success;
}

// void Deltronic_Send(unsigned long address)
// {
//     byte repeatTimes = 16;
//     byte repeat, index;
//     int periodLong, periodSync;
//     unsigned long bitmask;
//     int period = 640;

//     periodLong = 2 * period;
//     periodSync = 36 * period;

//     // Send seperator
//     digitalWrite(PIN_RF_TX_DATA, HIGH);
//     delayMicroseconds(period);

//     // Send sync
//     digitalWrite(PIN_RF_TX_DATA, LOW);
//     delayMicroseconds(periodSync);
//     digitalWrite(PIN_RF_TX_DATA, HIGH);
//     delayMicroseconds(period);

//     for (repeat = 0; repeat < repeatTimes; repeat++)
//     {
//         bitmask = 0x00000800L;
//         for (index = 0; index < 12; index++)
//         {
//             if (address & bitmask)
//             {
//                 // Send 1
//                 digitalWrite(PIN_RF_TX_DATA, LOW);
//                 delayMicroseconds(periodLong);
//                 digitalWrite(PIN_RF_TX_DATA, HIGH);
//                 delayMicroseconds(period);
//             }
//             else
//             {
//                 // Send 0
//                 digitalWrite(PIN_RF_TX_DATA, LOW);
//                 delayMicroseconds(period);
//                 digitalWrite(PIN_RF_TX_DATA, HIGH);
//                 delayMicroseconds(periodLong);
//             }
//             bitmask >>= 1;
//         }
//         // Send sync
//         digitalWrite(PIN_RF_TX_DATA, LOW);
//         delayMicroseconds(periodSync);
//         digitalWrite(PIN_RF_TX_DATA, HIGH);
//         delayMicroseconds(period);
//     }
//     digitalWrite(PIN_RF_TX_DATA, LOW);
// }

#endif // PLUGIN_083

