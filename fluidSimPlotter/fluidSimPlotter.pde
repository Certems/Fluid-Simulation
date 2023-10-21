ArrayList<ArrayList<PVector>> particlePositionData = new ArrayList<ArrayList<PVector>>();
ArrayList<ArrayList<PVector>> particleColourData   = new ArrayList<ArrayList<PVector>>();
float particleSize = 10.0;
int displayFrame   = 0;
int frameSpeed     = 1;
int frameFactor    = 1;

boolean paused = false;
boolean showHotkeys = true;

void setup()
{
    size(800,800);
    loadPositionData();
    loadColourData();
    frameRate(60);
}
void draw()
{
    background(30,30,30);
    display();
    if(showHotkeys){
        displayHotkeys();}
    displayFrameCounter();
    //Easy to add pause and play mechanics
}


void keyPressed()
{
    if(key == '1'){
        frameSpeed--;
    }
    if(key == '2'){
        frameSpeed++;
    }
    if(key == '3'){
        paused = !paused;
    }
    if(key == '4'){
        frameFactor--;
    }
    if(key == '5'){
        frameFactor++;
    }
    if(key == 'h'){
        showHotkeys = !showHotkeys;
    }
}


void loadPositionData()
{
    /*
    Loads data from the file generated by C++
    1. Read text file, bring into a set of frame data
    2. Go through each frame
    3. Go through that list until commas are reached, then separate into position data values
    3. Add those position data values to the particlePositionData 2D list, at the correct frame, preserving order

    Data supplied is formatted as such (in the case of 3 particles);
    1.5,4.3,9.2,0.3,0.5,7.3,      <-- Frame 1
    ...
    ...                           <-- Frame n
    |------|-------|-------|
     P1     P2      P3
    */
    particlePositionData.clear();   //Ensure this is empty first
    String[] frameDataSet = loadStrings("particlePositionData.txt");
    for(int j=0; j<frameDataSet.length; j++)
    {
        particlePositionData.add(new ArrayList<PVector>());
        String dataValue = "";
        int commaCount   = 0;   //Counts commas, ensures each vector gets its 2 coordinates
        PVector dataVector = new PVector(0.0, 0.0);
        for(int i=0; i<frameDataSet[j].length(); i++)
        {
            if(frameDataSet[j].charAt(i) == ','){
                commaCount++;
                if(commaCount == 1){
                    dataVector.x = float(dataValue);
                }
                if(commaCount == 2){
                    dataVector.y = float(dataValue);
                    commaCount   = 0;
                    particlePositionData.get(j).add(dataVector);
                    dataVector   = new PVector(0.0, 0.0);
                }
                dataValue = "";
            }
            else{
                dataValue = dataValue +frameDataSet[j].charAt(i);
            }
        }
    }
}
void loadColourData()
{
    particleColourData.clear();   //Ensure this is empty first
    String[] frameDataSet = loadStrings("particleColourData.txt");
    for(int j=0; j<frameDataSet.length; j++)
    {
        particleColourData.add(new ArrayList<PVector>());
        String dataValue = "";
        int commaCount   = 0;   //Counts commas, ensures each vector gets its 2 coordinates
        PVector dataVector = new PVector(0.0, 0.0, 0.0);
        for(int i=0; i<frameDataSet[j].length(); i++)
        {
            if(frameDataSet[j].charAt(i) == ','){
                commaCount++;
                if(commaCount == 1){
                    dataVector.x = float(dataValue);
                }
                if(commaCount == 2){
                    dataVector.y = float(dataValue);
                }
                if(commaCount == 3){
                    dataVector.z = float(dataValue);
                    commaCount   = 0;
                    particleColourData.get(j).add(dataVector);
                    dataVector   = new PVector(0.0, 0.0);
                }
                dataValue = "";
            }
            else{
                dataValue = dataValue +frameDataSet[j].charAt(i);
            }
        }
    }
}
void display()
{
    /*
    Displays all frames in motion
    Will loop when it reaches the end
    */
    displayParticles(displayFrame);
    if(!paused){
        updateFrame();}
}
void updateFrame()
{
    if(frameCount % frameFactor == 0){
        displayFrame += frameSpeed;}
    if(displayFrame >= particlePositionData.size()){
        displayFrame -= particlePositionData.size();}
    if(displayFrame < 0){
        displayFrame += particlePositionData.size();}
}
void displayParticles(int nFrame)
{
    /*
    Displays the nth frame (nFrame) of particles
    */
    pushMatrix();
    pushStyle();
    translate(width/2.0, height/2.0);    //Moves to display in the centre of the canvas (origin in centre)
    for(int i=0; i<particlePositionData.get(nFrame).size(); i++)
    {
        PVector particleColour = particleColourData.get(nFrame).get(i);
        noStroke();
        fill(particleColour.x, particleColour.y, particleColour.z, 100);
        //ellipse(particlePositionData.get(nFrame).get(i).x, particlePositionData.get(nFrame).get(i).y, 7.0*particleSize, 7.0*particleSize);  //Particle Cloud
        fill(particleColour.x, particleColour.y, particleColour.z);
        ellipse(particlePositionData.get(nFrame).get(i).x, particlePositionData.get(nFrame).get(i).y, particleSize, particleSize);          //Particle Point
    }
    popStyle();
    popMatrix();
}


void displayFrameCounter()
{
    pushStyle();
    fill(255,255,255);
    textSize(20);
    textAlign(CENTER,CENTER);
    text(displayFrame, 30, 20);
    popStyle();
}
void displayHotkeys()
{
    pushStyle();
    fill(255,255,255);
    textSize(15);
    textAlign(LEFT);
    text("1->   Speed-", 20,  50);
    text("2->   Speed+", 20,  70);
    text("3->    Pause", 20,  90);
    text("4-> sFactor-", 20, 110);
    text("4-> sFactor+", 20, 130);
    text("h->     Hide", 20, 150);
    popStyle();
}