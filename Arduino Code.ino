#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/kalpurush9pt8b.h>
#include <Fonts/kalpurush9pt7b.h>
#include <Fonts/RupaliAnsi9pt8b.h>
#include <Fonts/RupaliAnsiExtend9pt8b.h>
#include<avr/pgmspace.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(11, 10); //SIM800L Tx & Rx is connected to Arduino #11 & #10
String t="";
int q1;
int q2;
int ind1; 
int ind2;
int p;


void setup()
{
  Serial.begin(9600);
  
  mySerial.begin(9600);
  
    
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    for(;;); 
  }
  display.display();
  delay(2000); 
  display.clearDisplay();

  //for gsm
  Serial.println("Initializing..."); 
  delay(1000);

  mySerial.println("AT"); 
  updateSerial();
  
  mySerial.println("AT+CMGF=1\r"); 
  updateSerial();
  mySerial.println("AT+CNMI=2,2,0,0,0\r"); 
  updateSerial();
}

int pow(int base, int power)
{
    int val=1;

    for(int i=1; i<=power; i++)
        val*=base;

    return val;
}

int hex_to_dec(String s)
{   
    int val = 0;
    for(int i=3, j=0; i>=0; i--, j++)
    {
        int digit;
        if(s[i]>='A' && s[i]<='F')
            digit=(s[i]-'A')+10;
        else 
            digit=(s[i]-'0');
        val+=(digit*(pow(16, j)));
    }

    return (val);
}




void(* resetFunc) (void) = 0;



void loop()
{ 
    if(mySerial.available()>0){
     t = mySerial.readString();
    Serial.println(t); 
    delay(10);
      q1=t.indexOf('"',23);
      q2=t.indexOf('"',q1);
      ind1 = t.indexOf('"',q2+20);
      ind2= t.indexOf('\0',q2+20);
      int sz=((ind2-2)-(ind1+3))/4;
      int arr[sz]; 
      Serial.println(ind1+3);
      Serial.println(ind2);
    oled();
    for(int i=ind1+3,j=0; i<ind2-2; i+=4,j++)
  {
    String k;
    for(int l=i; l<i+4; l++)
     { k+=t[l]; }
    Serial.println(k);
    int vl=hex_to_dec(k);
    Serial.println(vl);
    arr[j]=vl;

  }

for(int i=0;i<sz;i++)
{
  if((arr[i]==2503)||(arr[i]==2504)||(arr[i]==2495)||(arr[i]==2507)||(arr[i]==2508))
  { 
    if(arr[i-2]!=2509)
    {
    int m=arr[i-1];
    arr[i-1]=arr[i];
    arr[i]=m;
  }
  else
  {
    int u=arr[i];
    arr[i]=arr[i-1];
    arr[i-1]=arr[i-2];
    arr[i-2]=arr[i-3];
    arr[i-3]=u;
  }
  }
}
for(int i=0;i<sz;i++)
{
  if(arr[i]==2509)
  {
    int m=arr[i-1];
    arr[i-1]=arr[i];
    arr[i]=m;
  }
}
  
  for(int i=0;i<sz;i++)
  {  
    Serial.println(arr[i]);
      display.setFont(&kalpurush9pt8b);
      
    if(arr[i]==2507)
     {
      if(arr[i+1]!=2509)
      {
      display.write(2503);
      display.write(arr[i+1]);
      display.write(2494);
      i++;
      continue;}
      else
     {
      display.write(2503);
      display.write(arr[i+2]);
      display.setFont(&RupaliAnsi9pt8b);
      display.write(168);
      display.setFont(&kalpurush9pt8b);
      display.write(2494);
      i=i+3;
      continue;
     }
      
    }
        if(arr[i]==2508)
    { 
      if(arr[i+1]!=2509)
    {
      display.write(2503);
      display.write(arr[i+1]);
      display.write(2519);
      i++;
      continue;
    }
    else
    {
      display.write(2503);
      display.write(arr[i+2]);
      display.setFont(&RupaliAnsi9pt8b);
      display.write(168);
      display.setFont(&kalpurush9pt8b);
      display.write(2519);
      i=i+3;
      continue; 
      
    }
    }
    //combined letter
     if(arr[i]==2509)
     { 
      if(arr[i+1]==2480)
      {
        //ref
        display.write(arr[i+2]);
        display.setFont(&RupaliAnsi9pt8b);
        display.write(169);
        i=i+2;
        continue;    
      }
      //zo fola
      if(arr[i+2]==2479)
      {
        display.write(arr[i+1]);
        display.setFont(&RupaliAnsi9pt8b);
        display.write(168);
        i=i+2;
        continue; 
      }
      
      if(((arr[i+1]==2472)||(arr[i+1]==2467)) && (arr[i+2]==2463))
      { 
        // n/N+T
        display.setFont(&RupaliAnsi9pt8b);
        display.write(200);
        i=i+2;
        continue;
      } 
            if((arr[i+1]==2472) && (arr[i+2]==2464))
      { 
        //n+TH
        display.setFont(&RupaliAnsi9pt8b);
        display.write(218);
        i=i+2;
        continue;
      }
        if((arr[i+1]==2467) && (arr[i+2]==2464))
      { 
        //N+TH
        display.setFont(&RupaliAnsi9pt8b);
        display.write(201);
        i=i+2;
        continue;
      }
            if((arr[i+1]==2472) && (arr[i+2]==2465))
      { 
        //n+D
        display.setFont(&RupaliAnsi9pt8b);
        display.write(219);
        i=i+2;
        continue;
      }
        if((arr[i+1]==2453) && (arr[i+2]==2453))
      { 
        //k+k
        display.setFont(&RupaliAnsi9pt8b);
        display.write(176);
        i=i+2;
        continue;
      }
              if((arr[i+1]==2453) && (arr[i+2]==2463))
      { 
        //k+T
        display.setFont(&RupaliAnsi9pt8b);
        display.write(177);
        i=i+2;
        continue;
      }
        if((arr[i+1]==2453) && (arr[i+2]==2468))
      { 
        //k+t
        display.setFont(&RupaliAnsi9pt8b);
        display.write(179);
        i=i+2;
        continue;
      }
       if((arr[i+1]==2453) && (arr[i+2]==2480))
      { 
        //k+r
        display.setFont(&RupaliAnsi9pt8b);
        display.write(181);
        i=i+2;
        continue;
      }
         if((arr[i+1]==2453) && (arr[i+2]==2487))
      { 
        //k+SH
        display.setFont(&RupaliAnsi9pt8b);
        display.write(182);
        i=i+2;
        continue;
      }
          if((arr[i+1]==2453) && (arr[i+2]==2488))
      { 
        //k+s
        display.setFont(&RupaliAnsi9pt8b);
        display.write(183);
        i=i+2;
        continue;
      }
       if((arr[i+1]==2453) && (arr[i+2]==2472))
      { 
        //k+n
        display.setFont(&RupaliAnsi9pt8b);
        display.write(180);
        i=i+2;
        continue;
      } 
      //r fola______________________________________________________________________
         if((arr[i+1]!=2453)&&(arr[i+1]!=2468)&&(arr[i+1]!=2477)&& (arr[i+2]==2480))
      { 
        display.write(arr[i+1]);
        display.setFont(&RupaliAnsi9pt8b);
        display.write(170);
        i=i+2;
        continue;
      }
      //L+L______________________________________________________________________
         if(arr[i+2]==2482)
      { 
        display.write(arr[i+1]);
        display.setFont(&RupaliAnsi9pt8b);
        display.write(172);
        i=i+2;
        continue;
      }
      //m juktoborno
               if((arr[i+2]!=2472)&&(arr[i+2]!=2475)&& (arr[i+1]==2478))
      { 
        display.setFont(&RupaliAnsi9pt8b);
        display.write(164);
        display.setFont(&kalpurush9pt8b);
        display.write(arr[i+2]);
        i=i+2;
        continue;
      }
      //b juktoborno
      if((arr[i+1]!=2470)&&(arr[i+2]==2476))
      { 
        display.write(arr[i+1]);
        display.setFont(&RupaliAnsi9pt8b);
        display.write(161);
        i=i+2;
        continue;
      }
      //v juktoborno
            if(arr[i+2]==2477)
      { 
        display.write(arr[i+1]);
        display.setFont(&RupaliAnsi9pt8b);
        display.write(162);
        i=i+2;
        continue;
      }
      //n zuktoborno
            if((arr[i+1]!=2453)&&(arr[i+1]!=2488)&&(arr[i+2]==2472))
      { 
        display.write(arr[i+1]);
        display.setFont(&RupaliAnsi9pt8b);
        display.write(232);
        i=i+2;
        continue;
      }
      //m zuktoborno
      if((arr[i+1]!=2470)&&(arr[i+1]!=2488)&&(arr[i+2]==2478))
      { 
        display.write(arr[i+1]);
        display.setFont(&RupaliAnsi9pt8b);
        display.write(167);
        i=i+2;
        continue;
      }
      //s zuktoborno
            if((arr[i+2]!=2463)&&(arr[i+2]!=2472)&&(arr[i+2]!=2475)&&(arr[i+1]==2488))
      { 
        display.setFont(&RupaliAnsi9pt8b);
        display.write(175);
        display.setFont(&kalpurush9pt8b);
        display.write(arr[i+2]);
        i=i+2;
        continue;
      }
      //SH zuktoborno
            if((arr[i+2]!=2467)&&(arr[i+2]!=2463)&&(arr[i+2]!=2464)&&(arr[i+2]!=2475)&&(arr[i+1]==2487))
      { 
        display.setFont(&RupaliAnsi9pt8b);
        display.write(174);
        display.setFont(&kalpurush9pt8b);
        display.write(arr[i+2]);
        i=i+2;
        continue;
      }
 
        if(arr[i+1]==2455)
      { 
        //g+g
        if(arr[i+2]==2455)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(185);
        i=i+2;
        continue;
        }
        //g+d
        if(arr[i+2]==2470)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(186);
        i=i+2;
        continue;
        }
         //g+dh
        if(arr[i+2]==2471)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(187);
        i=i+2;
        continue;
        } 
      }
      //umo
       if(arr[i+1]==2457)
      { 
        //umo+k
        if(arr[i+2]==2453)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(188);
        i=i+2;
        continue;
        }
        //umo+g
        if(arr[i+2]==2455)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(189);
        i=i+2;
        continue;
        }
      }
      //j
             if(arr[i+1]==2460)
      { 
        //j+j
        if(arr[i+2]==2460)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(190);
        i=i+2;
        continue;
        }
        //j+jh
        if(arr[i+2]==2461)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(192);
        i=i+2;
        continue;
        }
         //j+nio
        if(arr[i+2]==2462)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(193);
        i=i+2;
        continue;
        }
      }
      //nio
             if(arr[i+1]==2462)
      { 
        //nio+c
        if(arr[i+2]==2458)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(194);
        i=i+2;
        continue;
        }
        //nio+ch
        if(arr[i+2]==2459)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(195);
        i=i+2;
        continue;
        }
         //nio+j
        if(arr[i+2]==2460)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(196);
        i=i+2;
        continue;
        }
        //nio+jh
        if(arr[i+2]==2461)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(197);
        i=i+2;
        continue;
        }
      }
      //T+T
      if((arr[i+1]==2463) && (arr[i+2]==2463))
      { 
        display.setFont(&RupaliAnsi9pt8b);
        display.write(198);
        i=i+2;
        continue;
      }
      //D+D
      if((arr[i+1]==2465) && (arr[i+2]==2465))
      { 
        display.setFont(&RupaliAnsi9pt8b);
        display.write(199);
        i=i+2;
        continue;
      }
      //N+D
      if((arr[i+1]==2467) && (arr[i+2]==2465))
      { 
        display.setFont(&RupaliAnsi9pt8b);
        display.write(202);
        i=i+2;
        continue;
      }
      //t
             if(arr[i+1]==2468)
      { 
        //t+t
        if(arr[i+2]==2468)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(203);
        i=i+2;
        continue;
        }
        //t+th
        if(arr[i+2]==2469)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(204);
        i=i+2;
        continue;
        }
         //t+m
        if(arr[i+2]==2478)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(205);
        i=i+2;
        continue;
        }
        //t+r
        if(arr[i+2]==2480)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(206);
        i=i+2;
        continue;
        }
        //t+n
        if(arr[i+2]==2472)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(90);
        display.write(232);
        i=i+2;
        continue;
        }
      }

      
      //d
             if(arr[i+1]==2470)
      { 
        //d+d
        if(arr[i+2]==2470)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(207);
        i=i+2;
        continue;
        }
        //d+dh
        if(arr[i+2]==2471)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(215);
        i=i+2;
        continue;
        }
         //d+b
        if(arr[i+2]==2476)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(216);
        i=i+2;
        continue;
        }
        //d+m
        if(arr[i+2]==2478)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(217);
        i=i+2;
        continue;
        }
      }  
      //n
       if(arr[i+1]==2472)
      { 
        //n+dh
        if(arr[i+2]==2471)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(220);
        i=i+2;
        continue;
        }
        //n+s
        if(arr[i+2]==2488)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(221);
        i=i+2;
        continue;
        }
        //n+d_________________________________________________________________
        if(arr[i+2]==2470)
        {
        display.setFont(&RupaliAnsiExtend9pt8b);
        display.write(8250);
        display.setFont(&RupaliAnsi9pt8b);
        display.write(96);
        i=i+2;
        continue;
        } 
      }
      //p
             if(arr[i+1]==2474)
      { 
        //p+T
        if(arr[i+2]==2463)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(222);
        i=i+2;
        continue;
        }
        //p+t
        if(arr[i+2]==2468)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(223);
        i=i+2;
        continue;
        }
         //p+p
        if(arr[i+2]==2474)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(224);
        i=i+2;
        continue;
        }
        //p+s
        if(arr[i+2]==2488)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(225);
        i=i+2;
        continue;
        }
      }
      //b
             if(arr[i+1]==2476)
      { 
        //b+j
        if(arr[i+2]==2460)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(226);
        i=i+2;
        continue;
        }
        //b+d
        if(arr[i+2]==2470)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(227);
        i=i+2;
        continue;
        }
         //b+dh
        if(arr[i+2]==2471)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(228);
        i=i+2;
        continue;
        }
      }
      //v+r
      if((arr[i+1]==2477) && (arr[i+2]==2480))
      { 
        display.setFont(&RupaliAnsi9pt8b);
        display.write(229);
        i=i+2;
        continue;
      }
      //m
        if(arr[i+1]==2478)
      { 
        //m+n
        if(arr[i+2]==2472)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(230);
        i=i+2;
        continue;
        }
        //m+f
        if(arr[i+2]==2475)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(231);
        i=i+2;
        continue;
        }
      }
      
      //l
             if(arr[i+1]==2482)
      { 
        //l+k
        if(arr[i+2]==2453)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(233);
        i=i+2;
        continue;
        }
        //l+g
        if(arr[i+2]==2455)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(234);
        i=i+2;
        continue;
        }
         //l+T
        if(arr[i+2]==2463)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(235);
        i=i+2;
        continue;
        }
        //l+D
        if(arr[i+2]==2465)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(236);
        i=i+2;
        continue;
        }
         //l+p
        if(arr[i+2]==2474)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(237);
        i=i+2;
        continue;
        }
        //l+f
        if(arr[i+2]==2475)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(238);
        i=i+2;
        continue;
        }
      }

      //sh
        if(arr[i+1]==2486)
      { 
        //sh+c
        if(arr[i+2]==2458)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(240);
        i=i+2;
        continue;
        }
        //sh+ch
        if(arr[i+2]==2459)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(241);
        i=i+2;
        continue;
        }
      }
      //SH
        if(arr[i+1]==2487)
      { 
        //SH+N
        if(arr[i+2]==2467)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(242);
        i=i+2;
        continue;
        }
        //SH+T
        if(arr[i+2]==2463)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(243);
        i=i+2;
        continue;
        }
        //SH+TH
        if(arr[i+2]==2464)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(244);
        i=i+2;
        continue;
        }
        //SH+f
        if(arr[i+2]==2475)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(245);
        i=i+2;
        continue;
        }
      }
      //s
        if(arr[i+1]==2488)
      { 
        //s+kh
        if(arr[i+2]==2454)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(246);
        i=i+2;
        continue;
        }
        //s+T
        if(arr[i+2]==2463)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(247);
        i=i+2;
        continue;
        }
        //s+n
        if(arr[i+2]==2472)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(248);
        i=i+2;
        continue;
        }
        //s+f
        if(arr[i+2]==2475)
        {
        display.setFont(&RupaliAnsi9pt8b);
        display.write(249);
        i=i+2;
        continue;
        }
      }
      //h+n
       if((arr[i+1]==2489) && (arr[i+2]==2472))
      { 
        display.setFont(&RupaliAnsi9pt8b);
        display.write(253);
        i=i+2;
        continue;
      }
      //h+m
      if((arr[i+1]==2489) && (arr[i+2]==2478))
      { 
        display.setFont(&RupaliAnsi9pt8b);
        display.write(254);
        i=i+2;
        continue;
      }
      //R+g
       if((arr[i+1]==2524) && (arr[i+2]==2455))
      { 
        display.setFont(&RupaliAnsi9pt8b);
        display.write(255);
        i=i+2;
        continue;
      }
     }
      
        
  if((arr[i]==32)||(arr[i]==33)||(arr[i]==34)||(arr[i]==58)||(arr[i]==59)||(arr[i]==61)||(arr[i]==63)||(arr[i]==40)||(arr[i]==41)||(arr[i]==43)||(arr[i]==44)||(arr[i]==47))
  {   
      display.setFont(&kalpurush9pt7b);
      display.write(arr[i]);
      continue;
    }
 
    display.write(arr[i]);
    }  
  display.display();

      delay(12000);
      resetFunc();
    }
    
}

void oled()
{
   display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,30);
 // display.clearDisplay();
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());
  }
}
