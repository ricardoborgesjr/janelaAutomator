#include <ArduinoJson.h>
#include <Servo.h>

const int ldrPin = A0;
const int servoPin = 9;

const int capacity = JSON_OBJECT_SIZE(5) + 40;
StaticJsonDocument<capacity> docINPUT;
StaticJsonDocument<capacity> docOUTPUT;

Servo servo;


int valorLDR = 0;
int posicao_servo = 0; //posicao do servo motor
int posicao_servo_app = 0; //posicao do servo motor
int abertura = 0; //valor para abertura da claraboia
int fechamento = 1023; //valor para fechamento da clara boia
bool automatico = true;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  while (!Serial)
    continue;

  pinMode(ldrPin, INPUT);
  
  servo.attach(servoPin);
  
  servo.write(90);

  delay(500);

}

void loop() {
  // put your main code here, to run repeatedly:
  recebeConfig();

  if(automatico){
    aberturaAutomatico();
  }
  else{
    aberturaManual();
  }

  enviaConfig();
  delay(250);
  
  
}

void aberturaAutomatico(){
  valorLDR = analogRead(ldrPin);

  //Serial.print("sensor = " );
  //Serial.println(valorLDR); 

  posicao_servo = map(valorLDR, abertura, fechamento, 0, 90);

  servo.write(posicao_servo);

}

void aberturaManual(){
  servo.write(posicao_servo);
}

void enviaConfig(){
  JsonObject object = docOUTPUT.to<JsonObject>();
  
  object["ldr"] = valorLDR;
  object["servo"] = map(posicao_servo, 0, 90, 100, 0);
  object["abertura"] = abertura;
  object["fechamento"] = fechamento;
  object["auto"] = automatico;
  

  serializeJson(docOUTPUT, Serial);

}

void recebeConfig(){

  DeserializationError error = deserializeJson(docINPUT, Serial);

  if (!error) {
    int servo_tmp =  docINPUT["servo"];
    int abertura_tmp = docINPUT["abertura"];
    int fechamento_tmp = docINPUT["fechamento"];
    
    if(servo_tmp > 0 || abertura_tmp > 0 || fechamento_tmp > 0){
      posicao_servo = map(docINPUT["servo"], 0, 100, 90, 0);
      abertura = docINPUT["abertura"];
      fechamento = docINPUT["fechamento"];
      automatico = docINPUT["auto"];
      //Serial.println("Sucesso");
    }
  }
  /*else{
      Serial.println("Erro parse JSON "); 
      Serial.println(error.c_str());

  }*/
}
