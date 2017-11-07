// Define colour sensor LED pins
int ledArray[] = {3,5,6};

// Define visual output LED pins (R=9, G=11, B=10)
int ledOutArray[] = {9,10,11};

// Define photocell pin
int photoCell = A7;

// Boolean to know if the balance has been set
boolean balanceSet = false;

// Place holders for colour detected
int red = 0;
int green = 0;
int blue = 0;

// Floats to hold colour arrays
float colourArray[] = {0,0,0};
float whiteArray[] = {0,0,0};
float blackArray[] = {0,0,0};

float colourMax[] = {0,0,0};
float colourMin[] = {255,255,255};
float colourMapped[] = {0,0,0};

// Place holder for average
int avgRead;

void setup() {
  // Setup the outputs for the colour sensor
  pinMode(ledArray[0],OUTPUT);
  pinMode(ledArray[1],OUTPUT);
  pinMode(ledArray[2],OUTPUT);
  
  // Setup the outputs for the visual colourLED
  pinMode(ledOutArray[0],OUTPUT);
  pinMode(ledOutArray[1],OUTPUT);
  pinMode(ledOutArray[2],OUTPUT);

  // Setup photocell as input
  pinMode(photoCell, INPUT);
 
  // Begin serial communication
  Serial.begin(9600);
}

void loop() {
  checkBalance();
  checkColour();
  printColour();
  showColor();
}

void checkBalance() {
  // Check if the balance has been set, if not, set it
  if(balanceSet == false){
    setBalance();
  }
}

void setBalance() {
  blinkWhiteLeds();
  // Set white balance
  delay(5000);              // Delay for five seconds, this gives us time to get a white sample in front of our sensor
  
  // Scan the white sample, go through each light, get a reading, set the base reading for each colour red, green, and blue to the white array
  for(int i=0; i<=2; i++) {
    digitalWrite(ledArray[i], LOW);
    delay(100);
    getReading(5);          // Number is the number of scans to take for average, this whole function is redundant, one reading works just as well.
    whiteArray[i] = avgRead;
    digitalWrite(ledArray[i], HIGH);
    delay(100);             // Done scanning white, now it will pulse blue to tell you that it is time for the black (or grey) sample.
  }
  
  blinkWhiteLeds();
  // Set black balance
  delay(5000);              // Wait for five seconds so we can position our black sample 
  
  // Go ahead and scan, sets the colour values for red, green, and blue when exposed to black
  for(int i=0; i<=2; i++) {
    digitalWrite(ledArray[i], LOW);
    delay(100);
    
    getReading(5);
    blackArray[i] = avgRead;
    //blackArray[i] = analogRead(2);
    digitalWrite(ledArray[i], HIGH);
    delay(100);
  }
  // Set boolean value so we know that balance is set
  balanceSet = true;
  delay(5000);     // Delay another 5 seconds to let us catch up
}

void checkColour() {
  for(int i=0; i<=2; i++) {
    digitalWrite(ledArray[i],LOW);  // Turn on the LED, red, green or blue depending which iteration
    delay(100);                      // Delay to allow CdS to stabalize (they are slow)
    
    getReading(5);                   // Take a reading however many times
    colourArray[i] = avgRead;        // Set the current colour in the array to the average reading
    float greyDiff = whiteArray[i] - blackArray[i];                   // The highest possible return minus the lowest returns the area for values in between
    colourArray[i] = (colourArray[i] - blackArray[i])/(greyDiff)*255; // The reading returned minus the lowest value divided by the possible range multiplied by 
                                                                      // 255 will give us a value roughly between 0-255 representing the value for the current 
                                                                      // reflectivity (for the colour it is exposed to) of what is being scanned
    
    colourMapped[0] = map(colourArray[1],20,250,255,0);
    colourMapped[1] = map(colourArray[2],8,250,255,0);
    colourMapped[2] = map(colourArray[3],6,245,255,0);
    
    digitalWrite(ledArray[i],HIGH);   // Turn off the current LED
    delay(100);
  }
}

void getReading(int times) {
  int reading;
  int tally = 0;
  
  // Take the reading however many times was requested and add them up
  for(int i=0; i<times; i++) {
    reading = analogRead(photoCell);
    tally = reading + tally;
    delay(10);
  }
  
  // Calculate the average and set it
  avgRead = (tally)/times;
}

// Prints the colour in the colour array, in the next step, we will send this to processing to see how good the sensor works.
void printColour() {
  Serial.print("R = ");
  Serial.print(int(colourArray[0]));
  Serial.print(", ");
  Serial.println(int(colourMapped[0]));
  
  Serial.print("G = ");
  Serial.print(int(colourArray[1]));
  Serial.print(", ");
  Serial.println(int(colourMapped[1]));
  
  Serial.print("B = ");
  Serial.print(int(colourArray[2]));
  Serial.print(", ");
  Serial.println(int(colourMapped[2]));
  
  Serial.println();
  //delay(1000);
}

void showColor() {
  analogWrite(ledOutArray[0],int(colourMapped[0]));     // Red
  analogWrite(ledOutArray[1],int(colourMapped[1]));     // Blue
  analogWrite(ledOutArray[2],int(colourMapped[2]));     // Green
}

void blinkWhiteLeds() {
  delay(50);
  digitalWrite(ledArray[0],LOW);
  digitalWrite(ledArray[1],LOW);
  digitalWrite(ledArray[2],LOW);
  digitalWrite(ledOutArray[0],LOW);
  digitalWrite(ledOutArray[1],LOW);
  digitalWrite(ledOutArray[2],LOW);
  delay(50);
  digitalWrite(ledArray[0],HIGH);
  digitalWrite(ledArray[1],HIGH);
  digitalWrite(ledArray[2],HIGH);
  digitalWrite(ledOutArray[0],HIGH);
  digitalWrite(ledOutArray[1],HIGH);
  digitalWrite(ledOutArray[2],HIGH);
}

