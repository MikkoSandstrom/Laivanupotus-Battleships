#include <AlignedJoy.h>

#define PIN_JOY1_X   A1  
#define PIN_JOY1_Y   A2  
#define PIN_JOY2_X   A3  
#define PIN_JOY2_Y   A4  

AlignedJoy joystick1(PIN_JOY1_X, PIN_JOY1_Y);
AlignedJoy joystick2(PIN_JOY2_X, PIN_JOY2_Y);

#include <Adafruit_NeoPixel.h>

#define LED_PIN1 10
#define LED_PIN2 11
#define LED_PIN3 12
#define LED_PIN4 13
#define LED_COUNT 100
Adafruit_NeoPixel strip1=Adafruit_NeoPixel(LED_COUNT, LED_PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2=Adafruit_NeoPixel(LED_COUNT, LED_PIN2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip3=Adafruit_NeoPixel(LED_COUNT, LED_PIN3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip4=Adafruit_NeoPixel(LED_COUNT, LED_PIN4, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip; //Pointer, jolla viitataan funktioissa eri nauhoihin, jotta samalla funktiolla pystyy ohjaamaan molempia nauhoja
Adafruit_NeoPixel strip_opponent;
byte ledlist1[LED_COUNT];
byte ledlist2[LED_COUNT];
byte ledlist3[LED_COUNT];
byte ledlist4[LED_COUNT];// Listat, joiden alkioiden arvoiksi tulee 0-7 sen mukaan, minkä värinen kunkin ledin tulee olla
byte* ledlist; //Sama kuin ylempänä oleva "strip", viitataan listoihin
byte* ledlist_opponent;
int index = 0; //Ledien aloitusindeksi. Kertoo laivan ensimmäisen ledin sijainnin
int buttonstate =1; // Muuttuja, joka määrittää laivojen käännön 
const int led_rows = 10; // Matriisien rivien määrä
int add = LED_COUNT/led_rows; //Luku, joka lisätään indeksiin, kun liikutaan riviltä toiseen.
int gamestate = 1; //Muuttuja, jolla pidetään kirjaa, missä vaiheessa peli on
int ship_length = 5; //Laivojen aloituspituus
int ship_place; //Muuttuja, jolla tarkastetaan, asettiko pelaaja laivan.
int ship_3; //Muuttuja, joka päivittyy, kun 3:n pituinen laiva asetetaan ensimmäisen kerran
uint32_t led_colors[]={0,256,1,65536,257,65537,65792,65793}; //Käytettävissä olevat ledien värit, vihreä, sininen, punainen, syaani, lila, keltainen ja valkoinen
uint32_t brightness=1; 
uint32_t brightness_new;
int leftright;
int updown;
int enter;
int rotate;
int player1_hits=0;
int player2_hits=0;
int win[]={33,36,52,57,63,64,65,66};
int win_big[]={23,26,33,36,51,58,62,67,73,74,75,76};
int lose[]={33,36,62,67,53,54,55,56};
int lose_big[]={23,26,33,36,71,78,62,67,53,54,55,56};
int tie[]={33,36,53,54,55,56};
int tie_big[]={23,26,33,36,62,63,64,65,66,67};

void setup() {
  strip1.begin();
  strip2.begin();
  strip3.begin();
  strip4.begin();
  strip1.show();
  strip2.show();
  strip3.show();
  strip4.show();
  Serial.begin(9600);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP); 
  pinMode(3, INPUT_PULLUP);
}

int place_ships_main(int player){
  if(player==1){           
    ledlist = ledlist1;     //Viitataan pelaajan 1 nauhaan 
    strip = strip1;         //ja listaan
    rotate = digitalRead(2);//Luetaan pelaajan 1 kääntönapin tila       
  }                         
  if(player==2){
    ledlist = ledlist2;     //Viitataan pelaajan 2 nauhaan 
    strip = strip2;         //ja listaan
    rotate = digitalRead(8);//Luetaan pelaajan 2 kääntönapin tila
  }                          
  if(rotate == LOW){ //Jos kääntönappia on painettu
    delay(200);
    buttonstate = buttonstate*-1; //Vaihdetaan laivan suunta
    for(int j = 1;j<=ship_length-1;j++){
      if((index+j)%add==0 && buttonstate==1){ //Jos suunta vaihdetaan vaakasuoraksi ja aikaisempi pystysuora laiva on ollut oikeassa reunassa
        for(int j = 0; j<=ship_length-1; j++){
          strip.setPixelColor(index+j*add,led_colors[ledlist[index+j*add]]); //Palautetaan pystysuoran laivan alla olleet ledit takaisin omiin arvoihinsa
        }
        index--; //Liikutetaan indeksiä taaksepäin niin monta kertaa, että vaakasuora laiva mahtuu riville
      }
    }
    if(index>LED_COUNT-add*ship_length&&buttonstate == -1){//Jos suunta vaihdetaan pystysuoraksi ja aikaisempi vaakasuora laiva on ollut alimmalla rivillä
      for(int j = 0; j<=ship_length-1; j++){ 
        strip.setPixelColor(index+j,led_colors[ledlist[index+j]]); //Palautetaan vaakasuoran laivan alla olleet ledit takaisin omiin arvoihinsa
      }
      while(index>=LED_COUNT-add*(ship_length-1)){
        index-=add; //Liikutetaan indeksiä rivejä taaksepäin niin monta kertaa, että pystuora laiva mahtuu sarakkeelle
      }
    }
  }
  if(buttonstate==1){ //Vaakasuorat laivat
    if(ship_place==1){ //Pelaaja asetti laivan onnistuneesti
      for(int j=0;j<=ship_length-1; j++){ //Näytetään laivat nauhalla ja tallennetaan arvot listaan
        strip.setPixelColor(index+j, led_colors[1]);
        ledlist[index+j] = 1;
        strip.show();
      }
      index = 0; //Palautetaan indeksi 0:aan, jotta uusi laiva alkaa vasemmasta yläkulmasta. Voi ottaa pois käytöstä, jos haluaa jatkaa samasta pisteestä
      ship_length--; //Lyhennetään seuraavaa laivaa yhdellä
      if(ship_length==2&&ship_3==0){ //Jos aikaisempi laiva oli pituudeltaan 3, mutta toista samanpituista ei ole vielä asetettu. Arvoja voidaan vaihtaa, jos halutaan enemmän eri pituisia laivoja
        ship_length++; 
        ship_3++; 
      }
      ship_place=0; //Palautetaan takaisin alkuarvoon  
    }
    if(ship_length<2){ //Kun kaikki laivat on asetettu, palautetaan toisen pelaajan tarvitsevat muuttujat takaisin alkuarvoihinsa
      ship_length=5;
      ship_place=0;
      ship_3=0;
      gamestate++; //Liikutaan pelissä eteenpäin
      delay(150);
    } 
    else {
      index = place_ships_horizontal(index, player); //Funktio, jolla liikutetaan vaakasuoria laivoja. Parametrina nykyinen laivan ensimmäinen sijainti/indeksi ja pelaaja. Palautusarvona indeksi, johon viimeksi liikuttiin 
    }
  } 
  if(buttonstate==-1){ //Pystysuorat laivat
    if(ship_place==1){ //Pelaaja asetti laivan onnistuneesti
      for(int j=0;j<=ship_length-1; j++){ //Näytetään laivat nauhalla ja tallennetaan arvot listaan
        strip.setPixelColor(index+j*add, led_colors[1]);
        ledlist[index+j*add] = 1;
        strip.show();
      }
      index=0; //Palautetaan indeksi 0:aan, jotta uusi laiva alkaa vasemmasta yläkulmasta. Voi ottaa pois käytöstä, jos haluaa jatkaa samasta pisteestä
      ship_length--; //Lyhennetään seuraavaa laivaa yhdellä
      if(ship_length==2&&ship_3==0){ //Jos aikaisempi laiva oli pituudeltaan 3, mutta toista samanpituista ei ole vielä asetettu. Arvoja voidaan vaihtaa, jos halutaan enemmän eri pituisia laivoja
        ship_length++;
        ship_3++; 
      }
      ship_place=0; //Palautetaan takaisin alkuarvoon
    }
    if(ship_length<2){ //Kun kaikki laivat on asetettu, palautetaan toisen pelaajan tarvitsevat muuttujat takaisin alkuarvoihinsa
      ship_length=5;
      ship_place=0;
      ship_3=0;
      buttonstate=1; //Muutetaan arvo, jotta seuraavan pelaajan laivat alkavat vaakasuorina
      gamestate++; //Liikutaan pelissä eteenpäin
      delay(150); //Pieni viive
    }
   else {
    index = place_ships_vertical(index, player); //Funktio, jolla liikutetaan pystysuoria laivoja. Parametrina nykyinen laivan ensimmäinen sijainti/indeksi ja pelaaja. Palautusarvona indeksi, johon viimeksi liikuttiin 
   }
  }
  return index; //Palautetaan nykyinen indeksi/laivan sijainti
}


int place_ships_horizontal(int i,int player){ //Funktio, jolla liikutetaan vaakasuoria laivoja. Parametrina nykyinen laivan ensimmäinen sijainti/indeksi ja pelaaja. Palautusarvona indeksi, johon viimeksi liikuttiin
  if(player==1){                
    ledlist = ledlist1;         //Viitataan pelaajan 1 listaan
    strip = strip1;             //ja nauhaan
    leftright=joystick1.read(Y);//Luetaan pelaajan 1 joystickin arvo sivuttaiselle liikkeelle,
    updown=joystick1.read(X);   //joystickin arvo pystysuuntaiselle liikkeelle,
    enter = digitalRead(3);     //enter-napin tila
  } 
  if(player==2){                    
    ledlist = ledlist2;         //Viitataan pelaajan 1 listaan
    strip = strip2;             //ja nauhaan
    leftright=joystick2.read(Y);//Luetaan pelaajan 1 joystickin arvo sivuttaiselle liikkeelle,
    updown=joystick2.read(X);   //joystickin arvo pystysuuntaiselle liikkeelle,
    enter = digitalRead(9);     //enter-napin tila
  }                    
  for(int j = 0; j<=ship_length-1; j++){
    strip.setPixelColor(i+j*add,led_colors[ledlist[i+j*add]]); //Jos laivan suunta oli käännetty, mutta pystysuora laiva ei ollut oikeassa reunassa eli sitä ei tarvinnut liikuttaa. Palautetaan pystysuoran laivan alla olleet ledit takaisin omiin arvoihinsa
    strip.setPixelColor(i+j,led_colors[2]); //Asetetaan vaakasuora laiva indeksistä eteenpäin
  }
  strip.show();
  if(leftright<5){ //Liikutaan oikealle
    i++; //Liikutetaan laivaa yksi oikealle
    strip.setPixelColor(i-1,led_colors[ledlist[i-1]]); //Asetetaan aikasemman laivan ensimmäinen ledi takaisin omaan arvoonsa
    if((i+ship_length-1)%add==0){ //Estetään laivan liikkuminen osittain alemmalle riville
      i--;                          
    }  
    delay(100); //Viive vähentää nappien herkkyyttä
  }
  if (leftright>1000){ //Liikutaan vasemmalle
    i--; //Liikutetaan laivaa yksi vasemmalle
    strip.setPixelColor(i+ship_length,led_colors[ledlist[i+ship_length]]); //Asetetaan aikasemman laivan viimeinen ledi takaisin omaan arvoonsa
    if((i+1)%add==0){ //Estetään laivan liikkuminen osittain ylemmälle riville
      i++;
    }
    delay(100);
  }
  if (updown>1000){ //Liikutaan alas
    for(int j = 0; j<=ship_length-1;j++) { //Asetetaan aikasemman laivan ledit takaisin omaan arvoonsa
      strip.setPixelColor(i+j,led_colors[ledlist[i+j]]); 
    }
    i+=add; //Liikutetaan laivaa yksi rivi alaspäin
    if(i>=LED_COUNT-1){ // Estetään laiva menemästä alemmas kuin viimeinen rivi
      i-=add;
    }
    delay(100);
  }
  if(updown<200){ //Liikutaan ylös
    for(int j = 0; j<=ship_length-1;j++) { //Asetetaan aikasemman laivan ledit takaisin omaan arvoonsa
      strip.setPixelColor(i+j,led_colors[ledlist[i+j]]); 
    }
    i-=add; //Liikutetaan laivaa yksi rivi ylöspäin
    if(i<0){ // Estetään laiva menemästä ylemmäs kuin ensimmäinen rivi
      i+=add;
    }
    delay(100);
  } 
  if(enter ==LOW){ //Yritetään asettaa laiva
    delay(150);
    if(i<add){ //Halutaan asettaa laiva ensimmäiselle riville
      if(i==0){ //Kun halutun laivan ensimmäinen ledi on vasemmassa yläkulmassa
        for (int j=0;j<=ship_length; j++){ //Tarkistetaan alue laivan kohdalta, sen alapuolelta ja oikealta 
          if(ledlist[i+j]!=0|ledlist[i+j+add]!=0){ 
            return i; //Laivaa ei aseteta, jos halutun paikan ympärillä on jo laiva
          }                                                                                                           
        } 
      }
      else if((i+ship_length)%add==0){ //Kun halutun laivan viimeinen ledi on oikeassa yläkulmassa
        for (int j=-1;j<=ship_length-1; j++){ //Tarkistetaan alue laivan kohdalta, sen alapuolelta ja vasemmalta
          if(ledlist[i+j]!=0|ledlist[i+j+add]!=0){
            return i;
          }
        } 
      }
      else{
        for (int j=-1;j<=ship_length; j++){ //Haluttu laiva ei ole ylimmän rivin reunoissa 
          if(ledlist[i+j]!=0|ledlist[i+j+add]!=0){ //Tarkistetaan alue laivan kohdalta, sen alapuolelta, oikealta ja vasemmalta
            return i;
          }                                                                                                           
        }
      }   
    }
  else if((i>=LED_COUNT-add)){ //Halutaan asettaa laiva viimeiselle riville
      if(i%add==0){ //Kun halutun laivan ensimmäinen ledi on vasemmassa alakulmassa
        for (int j=0;j<=ship_length; j++){//Tarkistetaan alue laivan kohdalta, sen yläpuolelta ja oikealta 
          if(ledlist[i+j]!=0|ledlist[i+j-add]!=0){
             return i;
          }
        } 
      }
      else if((i+ship_length)%add==0){ //Kun halutun laivan viimeinen ledi on oikeassa alakulmassa
        for (int j = -1;j<=ship_length-1; j++){ //Tarkistetaan alue laivan kohdalta, sen yläpuolelta ja vasemmalta
          if(ledlist[i+j]!=0|ledlist[i+j-add]!=0){
            return i;
          }
        } 
      }
      else{
        for (int j=-1;j<=ship_length; j++){ //Haluttu laiva ei ole alimman rivin reunoissa 
          if(ledlist[i+j]!=0|ledlist[i+j-add]!=0){ //Tarkistetaan alue laivan kohdalta, sen yläpuolelta, oikealta ja vasemmalta
            return i;
          }
        }
      }   
    } 
    else{ //Halutaan asettaa laiva keskimmäisille riveille
      if(i%add==0){ //Kun halutun laivan ensimmäinen ledi on vasemmassa reunassa
        for (int j=0;j<=ship_length; j++){ //Tarkistetaan alue laivan kohdalta, sen alapuolelta, yläpuolelta ja oikealta
          if(ledlist[i+j]!=0|ledlist[i+j+add]!=0|ledlist[i+j-add]!=0){
            return i;
          }
        } 
      }
      else if((i+ship_length)%add==0){//Kun halutun laivan viimeinen ledi on oikeassa reunassa
        for (int j=-1;j<=ship_length-1; j++){// Tarkistetaan alue laivan kohdalta, sen alapuolelta, yläpuolelta ja vasemmalta
          if(ledlist[i+j]!=0|ledlist[i+j+add]!=0|ledlist[i+j-add]!=0){
            return i;
          }
        } 
      }
      else{ //Haluttu laiva ei ole keskimmäisten rivien reunoissa
        for ( int j=-1;j<=ship_length; j++){ // Tarkistetaan alue laivan kohdalta, sen alapuolelta, yläpuolelta, oikealta ja vasemmalta
          if(ledlist[i+j]!=0|ledlist[i+j+add]!=0|ledlist[i+j-add]!=0){
            return i;
          }
        }
      }  
    }
    ship_place = 1; //Jos halutun laivan ympärillä ei ole muita laivoja, vaihdetaan muuttuja, jotta aikaisempi funktio asettaa laivan
  }
  return i; //Palautetaan aikaisemmalle funktiolle nykyinen indeksi
}
  
  
int place_ships_vertical(int i, int player){ //Funktio, jolla liikutetaan pystysuoria laivoja. Parametrina nykyinen laivan ensimmäinen sijainti/indeksi ja pelaaja. Palautusarvona indeksi, johon viimeksi liikuttiin
  ship_place = 0;       
  if(player==1){                
    ledlist = ledlist1;         //Viitataan pelaajan 1 listaan
    strip = strip1;             //ja nauhaan
    leftright=joystick1.read(Y);//Luetaan pelaajan 1 joystickin arvo sivuttaiselle liikkeelle,
    updown=joystick1.read(X);   //joystickin arvo pystysuuntaiselle liikkeelle,
    enter = digitalRead(3);     //enter-napin tila
  } 
  if(player==2){                    
    ledlist = ledlist2;         //Viitataan pelaajan 1 listaan
    strip = strip2;             //ja nauhaan
    leftright=joystick2.read(Y);//Luetaan pelaajan 1 joystickin arvo sivuttaiselle liikkeelle,
    updown=joystick2.read(X);   //joystickin arvo pystysuuntaiselle liikkeelle,
    enter = digitalRead(9);     //enter-napin tila
  }                    
  for(int j = 0; j<=ship_length-1; j++){ 
    strip.setPixelColor(i+j,led_colors[ledlist[i+j]]); // Jos laivan suunta oli käännetty, mutta vaakasuora laiva ei ollut alimmilla riveillä eli sitä ei tarvinnut liikuttaa. Palautetaan vaakasuoran laivan alla olleet ledit takaisin omiin arvoihinsa
    strip.setPixelColor(i+j*add,led_colors[2]); // Asetetaan pystysuora laiva indeksistä alaspäin
  }  
  strip.show();
  if(leftright<5){ //Liikutaan oikealle
    for(int j = 0; j<=ship_length-1;j+=1) { //Asetetaan aikasemman laivan ledit takaisin omaan arvoonsa
      strip.setPixelColor(i+j*add,led_colors[ledlist[i+j*add]]); 
    }
    i++; //Liikutetaan laivaa yksi oikealle
    if((i)%add==0){ //Estetään laivan liikkuminen osittain alemmalle riville
      i--;                          
    }  
    delay(100); //Viive vähentää nappien herkkyyttä
  }
  if (leftright>1000){ //Liikutaan vasemmalle
    for(int j = 0; j<=ship_length-1;j+=1) { //Asetetaan aikasemman laivan ledit takaisin omaan arvoonsa
     strip.setPixelColor(i+j*add,led_colors[ledlist[i+j*add]]); 
    }
    i--;
    if((i+1)%add==0||i<0){ //Estetään laivan liikkuminen osittain ylemmälle riville
      i++;
    }
    delay(100);
  }
  if (updown>1000){ //Liikutaan alas
    strip.setPixelColor(i,led_colors[ledlist[i]]); 
    i+=add; //Liikutetaan laivaa yksi rivi alaspäin
    if(i>=LED_COUNT-(ship_length-1)*add){ // Estetään laiva menemästä alemmas kuin viimeinen rivi
      i-=add;
    }
    delay(100);
  }
  if(updown<200){ //Liikutaan ylös
    strip.setPixelColor(i+(ship_length-1)*add,led_colors[ledlist[i+(ship_length-1)*add]]); 
    i-=add; //Liikutetaan laivaa yksi rivi ylöspäin
    if(i<0){ // Estetään laiva menemästä ylemmäs kuin ensimmäinen rivi
      i+=add;
    }
    delay(100);
  } 
  if(enter ==LOW){ //Yritetään asettaa laiva
    delay(150);
    if(i%add==0){ //Halutaan asettaa laiva ensimmäiseen sarakkeeseen
      if(i==0){ //Kun halutun laivan ensimmäinen ledi on vasemmassa yläkulmassa
        for (int j=0;j<=ship_length; j++){ //Tarkistetaan alue laivan kohdalta, sen alapuolelta ja oikealta 
          if(ledlist[i+j*add]!=0|ledlist[i+j*add+1]!=0){ 
           return i; //Laivaa ei aseteta, jos halutun paikan ympärillä on jo laiva
          }                                                                                                           
        } 
      }
      else if((i+(ship_length-1)*add)==LED_COUNT-add){ //Kun halutun laivan viimeinen ledi on vasemmassa alakulmassa
        for (int j=-1;j<ship_length-1; j++){ //Tarkistetaan alue laivan kohdalta, sen yläpuolelta ja oikealta
          if(ledlist[i+j*add]!=0|ledlist[i+j*add+1]!=0){
            return i;
          }
        } 
      }
      else{
        for (int j=-1;j<=ship_length; j++){ //Haluttu laiva ei ole ensimmäisen sarakkeen kulmissa 
          if(ledlist[i+j*add]!=0|ledlist[i+j*add+1]!=0){ //Tarkistetaan alue laivan kohdalta, sen alapuolelta, yläpuolelta ja oikealta
           return i;
          }                                                                                                           
        }
      }   
    }
    else if((i+1)%add==0){ //Halutaan asettaa laiva viimeiselle sarakkeelle
      if(i==add-1){ //Kun halutun laivan ensimmäinen ledi on oikeassa yläkulmassa
        for (int j=0;j<=ship_length; j++){//Tarkistetaan alue laivan kohdalta, sen alapuolelta ja vasemmalta 
          if(ledlist[i+j*add]!=0|ledlist[i+j*add-1]!=0){
            return i;
          }
        } 
      }
      else if((i+(ship_length-1)*add)==LED_COUNT-1){ //Kun halutun laivan viimeinen ledi on oikeassa alakulmassa
        for (int j = -1;j<=ship_length-1; j++){ //Tarkistetaan alue laivan kohdalta, sen yläpuolelta ja vasemmalta
          if(ledlist[i+j*add]!=0|ledlist[i+j*add-1]!=0){
           return i;
          }
        } 
      }
      else{
        for (int j=-1;j<=ship_length; j++){ //Haluttu laiva ei ole viimeisen sarakkeen kulmissa 
          if(ledlist[i+j*add]!=0|ledlist[i+j*add-1]!=0){ //Tarkistetaan alue laivan kohdalta, sen alapuolelta, yläpuolelta ja vasemmalta
           return i;
          }
        }
      }   
    } 
    else{ //Halutaan asettaa laiva keskimmäisille sarakkeille
      if(i%add==0){ //Kun halutun laivan ensimmäinen ledi on ylimmällä rivillä
        for (int j=0;j<=ship_length; j++){ //Tarkistetaan alue laivan kohdalta, sen alapuolelta, oikealta ja vasemmalta
          if(ledlist[i+j*add]!=0|ledlist[i+j*add+1]!=0|ledlist[i+j*add-1]!=0){
            return i;
          }
        } 
      }
      else if(i+(ship_length-1)*add>=LED_COUNT-add){//Kun halutun laivan viimeinen ledi alimmalla rivillä
        for (int j=-1;j<=ship_length-1; j++){// Tarkistetaan alue laivan kohdalta, yläpuolelta, oikealta ja vasemmalta
          if(ledlist[i+j]!=0|ledlist[i+j*add+1]!=0|ledlist[i+j*add-1]!=0){
            return i;
          }
        } 
      }
      else{ //Haluttu laiva ei ole keskimmäisten sarakkeiden reunoissa
        for ( int j=-1;j<=ship_length; j++){ // Tarkistetaan alue laivan kohdalta, sen alapuolelta, yläpuolelta, oikealta ja vasemmalta
          if(ledlist[i+j*add]!=0|ledlist[i+j*add+1]!=0|ledlist[i+j*add-1]!=0){
            return i;
          }
        }
      }  
    }
    ship_place = 1; //Jos halutun laivan ympärillä ei ole muita laivoja, vaihdetaan muuttuja, jotta aikaisempi funktio asettaa laivan
  }
  return i; //Palautetaan aikaisemmalle funktiolle nykyinen indeksi
}


void turn(int player){//Pelikierros, jossa pelaaja ampuu
 if(player==1){
   strip = strip3;              //Viitataan pelaajan 1 ylänäyttöön,
   ledlist = ledlist3;          //sen listaan
   strip_opponent = strip2;     //sekä vastustajan nauhaan
   ledlist_opponent = ledlist2; //ja listaan
   leftright=joystick1.read(Y); //Luetaan pelaajan 1 joystickin arvo sivuttaiselle liikkeelle,
   updown=joystick1.read(X);    //pystysuoralle liikkeelle,
   enter = digitalRead(3);      //enter-napin tila
 }
 if(player==2){
  strip = strip4;              //Viitataan pelaajan 1 ylänäyttöön,
  ledlist = ledlist4;          //sen listaan
  strip_opponent = strip1;     //sekä vastustajan nauhaan
  ledlist_opponent = ledlist1; //ja listaan
  leftright=joystick2.read(Y); //Luetaan pelaajan 1 joystickin arvo sivuttaiselle liikkeelle,
  updown=joystick2.read(X);    //pystysuoralle liikkeelle,
  enter = digitalRead(9);      //enter-napin tila
 }
 strip.setPixelColor(index,led_colors[7]);//Asetetaan liikutteva ledi valkoiseksi
 strip.show();
 strip_opponent.show();
  if(leftright>1000){ //Vasemmalle liikkuminen
    delay(100);
    strip.setPixelColor(index,led_colors[ledlist[index]]); //Asetetaan alla oleva ledi omaan arvoonsa
    if(index%add==0){ //Estetään ledin liikkuminen rivin yli
      index++;
    }
    index--;  //Liikutaan vasemmalle
  }
  if(leftright<5){  //Oikealle liikkuminen
    delay(100);
    strip.setPixelColor(index,led_colors[ledlist[index]]); //Asetetaan alla oleva ledi omaan arvoonsa  
    index++;  //Liikutaan oikealla
    if(index%add==0){ //Estetään ledin liikkuminen rivin yli
      index--;
    }    
  }
  if(updown>1000){ //Alaspäin liikkumine
  delay(100);
  strip.setPixelColor(index,led_colors[ledlist[index]]); //Asetetaan alla oleva ledi omaan arvoonsa 
  index+=add; //Liikutaan Alaspäin
    if(index>=LED_COUNT){ //Estetään ledin liikkuminen viimeisen rivin alle
      index-=add;
    }  
  }
  if(updown<200){ // Ylöspäin liikkuminen
  delay(100);
  strip.setPixelColor(index,led_colors[ledlist[index]]);  //Asetetaan alla oleva ledi omaan arvoonsa
  index-=add; //Liikutaan ylöspäin
    if(index<0){  //Estetään ledin liikkuminen ensimmäisen rivin yli
      index+=add;
    }  
  }
  if(enter==LOW){ //Jos enter-nappia on painettu
    delay(200);
    if(ledlist[index]==3||ledlist[index]==4){ //Jos haluttuun kohtaan on jo ammuttu, ei tehdä mitään
    }
    else if(ledlist_opponent[index]==1){ //Jos halutussa kohdassa on vihollisen laiva
      strip_opponent.setPixelColor(index,led_colors[3]);  //Muutetaan vastustajan laiva näytöllä punaiseksi
      ledlist_opponent[index]=3;   
      strip.setPixelColor(index,led_colors[3]); //Muutetaan vastustajan laiva omalla ylänäytöllä punaiseksi
      ledlist[index]=3;
      strip.show();
      strip_opponent.show();
      index=0;  //Indeksi nollaan, jotta vastustaja aloittaa vuoronsa vasemmasta yläkulmasta
      if (player==1){ 
        gamestate++;  //Siirrytään pelaajan 2 vuoroon
        player1_hits++; //Lisätään pelaajalle 1 osuma
      }
      else{
        gamestate--;  //Siirrytään pelaajan 1 vuoroon
        player2_hits++; //Lisätään pelaajalle 2 osuma
      }
    }
    else{ //Muuten, jos halutussa kohdassa ei ole vastustajan laivaa eli huti
      strip_opponent.setPixelColor(index,led_colors[4]);  //Muutetaan kohta vastustajan näytöllä siniseksi
      ledlist_opponent[index]=4;
      strip.setPixelColor(index,led_colors[4]); //Muutetaan kohta omalla ylänäytöllä siniseksi
      ledlist[index]=4;
      strip.show();
      strip_opponent.show();
      index=0;  //Indeksi nollaan, jotta vastustaja aloittaa vuoronsa vasemmasta yläkulmasta
      if (player==1){
        gamestate++; //Siirrytään pelaajan 2 vuoroon
      }
      else{
        gamestate--;  //Siirrytään pelaajan 21vuoroon
      }
    }
  }
}


void brightness_set(){ //Funktio, jolla muokataan ledien kirkautta
  for(int k=1;k<=7;k++){
    led_colors[k]=led_colors[k]/brightness*brightness_new; //Käydään läpi kaikki saatavilla olevat värit, jaetaan vanhalla kirkkaudella ja kerrotaan uudella, jolloin saadaan uusi 10-desimaalinen väriarvo
  }
  for(int k=0;k<=LED_COUNT-1;k++){  //Päivitetään kaikki ledit, jotka ovat päällä uuteen kirkkauteen
    if(led_colors[ledlist1[k]]!=0){
      strip1.setPixelColor(k,led_colors[ledlist1[k]]);
    }
    if(led_colors[ledlist2[k]]!=0){
      strip2.setPixelColor(k,led_colors[ledlist2[k]]);
    }
    if(led_colors[ledlist3[k]]!=0){  
      strip3.setPixelColor(k,led_colors[ledlist3[k]]); 
    }
    if(led_colors[ledlist4[k]]!=0){  
      strip4.setPixelColor(k,led_colors[ledlist4[k]]); 
    }
  }
  brightness=brightness_new; //Asetetaan uusi kirkkaus vanhaksi seuraava säätöä varten
}

 
void loop() { 
  int sensorValue = analogRead(A0); //Luetaan kirkkaudensäädön säätövastuksen arvo
  brightness_new = map(sensorValue, 1, 1023, 1, 255 ); //Muutetaan saatu arvo välille 1-255
  if(brightness+1<brightness_new||brightness-1>brightness_new){ //Jos säätövastuksen arvo on muuttunut, kutsutaan kirkkaudensäätö-funktiota ja päivitetään nauhat
    brightness_set();
    strip1.show();
    strip2.show();
    strip3.show();
    strip4.show();
  }
  if(player1_hits==17&&player2_hits==17){ //Tasapeli
    gamestate = 5; 
  }
  else if(player1_hits==17&&player2_hits<=16&&gamestate==3){//Pelaaja 1 voittaa. Mahdollisuus, että Pelaaja 2 tasoittaa 
    gamestate=6;
  }
  else if(player2_hits==17){ //Pelaaja 2 voittaa
    gamestate=7;
  }
  switch (gamestate){ //Suorittaa eri caset riippuen gamestate arvosta
    case 1:
    {
      index = place_ships_main(1); //Kutsuu funktiota, jolla pelaaja 1 asettaa laivat.
      break; 
    }
    
    case 2:
    {
      index = place_ships_main(2); //Pelaaja 2 asettaa laivat
      break;
    }
    
    case 3:
    {
     turn(1); //Pelaajan 1 vuoro
     break;
    }
    
    case 4: //Pelaajan 2 vuoro
    {      
      turn(2);
      break;
    }
    case 5: //Peli päättyy, tasapeli
    {      
      strip3.clear(); //Nollataan ylänäytöt
      strip4.clear();
      for(int j=0;j<6;j++){ //Päivitettään nauhoille pieni tasapeli-hymiö
        strip3.setPixelColor(tie[j],led_colors[7]);
        strip4.setPixelColor(tie[j],led_colors[7]);
      }
      strip3.show();  
      strip4.show();
      strip3.clear();
      strip4.clear();
      delay(300);
      for(int j=0;j<10;j++){  //Päivitettään nauhoille iso tasapeli-hymiö
        strip3.setPixelColor(tie_big[j],led_colors[7]);
        strip4.setPixelColor(tie_big[j],led_colors[7]);
      }
      strip3.show();
      strip4.show();
      delay(300);
      break;
    }
    case 6: //Peli päättyy, Pelaaja 1 voittaa
    {      
      strip3.clear(); //Nollataan ylänäytöt
      strip4.clear();
      for(int j=0;j<8;j++){ //Päivitettään nauhoille pienet voitto- sekä häviö-hymiöt
        strip3.setPixelColor(win[j],led_colors[1]);
        strip4.setPixelColor(lose[j],led_colors[3]);
      }
      strip3.show();
      strip4.show();
      strip3.clear();
      strip4.clear();
      delay(300);
      for(int j=0;j<12;j++){  //Päivitettään nauhoille isot voitto- sekä häviö-hymiöt
        strip3.setPixelColor(win_big[j],led_colors[1]);
        strip4.setPixelColor(lose_big[j],led_colors[3]);
      }
      strip3.show();
      strip4.show();
      delay(300);
      break;
    }
    case 7: //Peli päättyy, Pelaaja 2 voittaa
    {      
      strip3.clear(); //Nollataan ylänäytöt
      strip4.clear();
      for(int j=0;j<8;j++){ //Päivitettään nauhoille pienet voitto- sekä häviö-hymiöt
        strip4.setPixelColor(win[j],led_colors[1]);
        strip3.setPixelColor(lose[j],led_colors[3]);
      }
      strip3.show();
      strip4.show();
      strip3.clear();
      strip4.clear();
      delay(300);
      for(int j=0;j<12;j++){  //Päivitettään nauhoille isot voitto- sekä häviö-hymiöt
        strip4.setPixelColor(win_big[j],led_colors[1]);
        strip3.setPixelColor(lose_big[j],led_colors[3]);
      }
      strip3.show();
      strip4.show();
      delay(300);
      break;
    }
  }
}
