/*C++ CONSOLE TRAINER ENJOY*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <Windows.h>
#include <string>
#include <ctime> // needed for our timer clock

#define XCOORD 0
#define YCOORD 1
#define ZCOORD 2
#define SAVE_LOCATION_1 VK_F4
#define SAVE_LOCATION_2 VK_F5
#define LOAD_LOCATION_1 VK_F6
#define LOAD_LOCATION_2 VK_F7
#define LOCATION_1 0
#define LOCATION_2 1
#define I_KEY 0x49
#define J_KEY 0x4A
#define K_KEY 0x4B
#define L_KEY 0x4C
#define U_KEY 0x55
#define O_KEY 0x4F
#define M1_KEY 0x01
#define Q_KEY 0x51
#define PI 3.14159265

void getPlayerHealth(HANDLE hProcHandle);
void WriteToMemory(HANDLE hProcHandle);
DWORD FindDmaAddy(int PointerLevel, HANDLE hProcHandle, DWORD Offsets[], DWORD BaseAddress);
DWORD GetCoordinate(HANDLE hProcHandle, int coordinate);
DWORD GetPlayerCoordinate(HANDLE hProcHandle, DWORD playeraddress, int coordinate);
DWORD GetHealth(HANDLE hProcHandle);
void WriteCoordinates(HANDLE hProcHandle, int locationNumber);
void WriteCoordinate (HANDLE hProcHandle, int coordinate, float valueToWrite);
float addToCoordinate(DWORD coordinate, float valueToAdd);
DWORD GetAngle(HANDLE hProcHandle, int direction);
DWORD getClosestPlayer(HANDLE hProcHandle);
void SetAngle(HANDLE hProcHandle, int direction, float angle);
void aimAtPlayer(HANDLE hProchHandle, int player, bool tele);
//CREATES the string used to determine the name of our target window
std::string GameName = "AssaultCube";
LPCSTR LGameWindow = "AssaultCube"; //<- MAKE SURE it matches the window name
std::string GameStatus;
//FUNCTION PROTOTYPES
bool IsGameAvail;
bool UpdateOnNextRun; //used to update the display menu only when something changed

//-------AMMO VARS--------
//number we are going to overwrite the current ammo with in bytes
bool AmmoStatus; // used to DEFine wether ammo is on or not
BYTE AmmoValue[] = {0x64,0x00,0x00,0x00};
DWORD AmmoBaseAddress = {0x004DF73C};
DWORD AmmoOffsets[] = {0x378, 0x14, 0x0}; //3 LEVEL pointer

//-------HEALTH VARS--------
bool HealthStatus; // used to DEFine wether ammo is on or not
BYTE HealthValue[] ={0x39,0x5,0x0,0x0};
DWORD HealthBaseAddress = {0x004DF73C};
DWORD HealthOffsets[] = {0xF4}; // 1 level pointer

//-------Location VARS--------
bool TeleportStatus; // used to DEFine whether teleport is allowed
bool ableToMove; // used to DEFine whether you can move using i, j, k and l
DWORD LocationBaseAddress = {0x004DF73C};
DWORD LocationOffsets[] = {0x34,0x3C,0x38}; // 1 level pointer
bool iPressed;
bool jPressed;
bool kPressed;
bool lPressed;	
bool uPressed;
bool oPressed;
bool qPressed;
DWORD TeleportLocationX1;
DWORD TeleportLocationY1;
DWORD TeleportLocationZ1;
DWORD TeleportLocationX2;
DWORD TeleportLocationY2;
DWORD TeleportLocationZ2;
DWORD location1[] = {TeleportLocationX1, TeleportLocationY1, TeleportLocationZ1};
DWORD location2[] = {TeleportLocationX2, TeleportLocationY2, TeleportLocationZ2};
DWORD *positions[] = {location1,location2};
DWORD AngleOffsets[] = {0x40,0x44}; // 1 level pointer
float hangle;
float vangle;

// Blink vars
bool ableToBlink;
bool m1Pressed;

int main() {

    //Declare our handles as NULL to avoid crashes when closing if they were unused e.g. player starts trainer and closes it before doing any cheats
    HWND hGameWindow = NULL;
    int timeSinceLastUpdate = clock(); //forces status update every x seconds
    int GameAvailTMR = clock();
    int OnePressTMR;//used to limit keys input to only one per x ms
    DWORD dwProcId = NULL;
    HANDLE hProcHandle = NULL;
    UpdateOnNextRun = true;
    std::string sAmmoStatus;
    std::string sHealthStatus;
    std::string sTeleportStatus;
    std::string iStatus;
	std::string sBlinkStatus;
    sAmmoStatus = "OFF";
    sHealthStatus = "OFF";
    sTeleportStatus = "OFF";
    iStatus = "OFF";
	sBlinkStatus = "OFF";
    OnePressTMR = clock();
    while(!GetAsyncKeyState(VK_INSERT)) { //Key is not = 'INSERT'
        // Does a series of checks every x ms and
        // checks that the game is available and capable of being
        // written to, if thats the case we write declare it available
        // otherwise we report where it went wrong
        // e.g. if game is closed we make things unavailable, or if its opened
        // we make options available again

        if(clock() - GameAvailTMR > 1000) {
			getClosestPlayer(hProcHandle);
            GameAvailTMR = clock();
            // Declare game unavailable by default
            // if it is available then it will change immediately
            IsGameAvail = false;
            // Check a valid window is available
            // Get Window Handle
            hGameWindow = FindWindow( NULL, LGameWindow);
            if (hGameWindow) {
                GetWindowThreadProcessId (hGameWindow, &dwProcId);
                //If it is a valid id we continue to try and open the process
                if (dwProcId != 0) {
                    // Get Process Handle
                    hProcHandle = OpenProcess (PROCESS_ALL_ACCESS, FALSE, dwProcId);
                    if (hProcHandle == INVALID_HANDLE_VALUE || hProcHandle == NULL) {
                        GameStatus = "Failed to open process for valid handle";
                    } else {
                        GameStatus = "AssaultCube Ready to hack";
                        IsGameAvail = true;
                    }
                } else {
                    GameStatus = "Failed to obtain process id";
                }
            } else { 
                GameStatus = "AssaultCube NOT FOUND";
            }

            // if UpdateNextRun is called or a number of seconds without updates have gone by an auto update is done
            // to make sure game is available etc.
            if (UpdateOnNextRun || clock() - timeSinceLastUpdate > 5000)
            {
				
                system("cls");
                std::cout << "----------------------------------------------------" << std::endl;
                std::cout << "        AssaultCube memory hacker" << std::endl;
                std::cout << "----------------------------------------------------" << std::endl << std::endl;
                std::cout << "GAME STATUS:"<< GameStatus  <<"   " << std::endl << std::endl;
                std::cout << "[F1] Unlimited ammo ->"<< sAmmoStatus <<"<-" << std::endl<< std::endl;
                std::cout << "[F2] Unlimited Health and armor ->" << sHealthStatus << "<-" << std::endl<< std::endl;
                std::cout << "[F3] Save Location ->" << sTeleportStatus << "<-" << std::endl<< std::endl;
                std::cout << "[INSERT] Exit" << std::endl;
                std::cout << "[F8] Move with I,J,K,L ->" << iStatus << "<-" << std::endl;
				std::cout << "[F9] Blink Strike ->" << sBlinkStatus << "<-" << std::endl;
				
				getPlayerHealth(hProcHandle);
		
                UpdateOnNextRun = false;
                timeSinceLastUpdate = clock();
            }
		
            if (IsGameAvail) {
                WriteToMemory (hProcHandle);
            }
        }


        // Stops Keys from being spammed e.g. only allow them to be pressed every x milliseconds
        if (clock() - OnePressTMR > 400) {
            if (IsGameAvail) {
                // DETECTS WHICH KEYS HAVE BEEN PRESSED IN order to turn cheats on and off
                if (GetAsyncKeyState (VK_F1)) {
                    OnePressTMR = clock();
                    // Reverts the ammo status e.g. from true to false and vice versa
                    AmmoStatus = !AmmoStatus;
                    UpdateOnNextRun = true;
                    // changes the text to update on next display
                    if (AmmoStatus) {
                        sAmmoStatus = "ON";
                    } else { 
                        sAmmoStatus = "OFF";
                    }
                } else if (GetAsyncKeyState(VK_F2)) {
                    OnePressTMR = clock();
                    HealthStatus = !HealthStatus;
                    UpdateOnNextRun = true;
                    // changes the text to update on next display
                    if (HealthStatus) {
                        sHealthStatus = "ON";
                    } else { 
                        sHealthStatus = "OFF";
                    }
                } else if (GetAsyncKeyState(VK_F3)) {
                    OnePressTMR = clock();
                    TeleportStatus = !TeleportStatus;
                    UpdateOnNextRun = true;
                    // changes the text to update on next display
                    if (TeleportStatus) {
                        sTeleportStatus = "ON";
                    } else {
                        sTeleportStatus = "OFF";
                    }
                } else if (GetAsyncKeyState(VK_F8)) {
                    OnePressTMR = clock();
                    UpdateOnNextRun = true;
                    ableToMove = !ableToMove;
                    if (ableToMove) {
                        iStatus = "ON";
                    } else {
                        iStatus = "OFF";
                    }
                }else if (GetAsyncKeyState(VK_F9)) {
					OnePressTMR = clock();
                    UpdateOnNextRun = true;
                    ableToBlink = !ableToBlink;
                    if (ableToBlink) {
                        sBlinkStatus = "ON";
                    } else {
                        sBlinkStatus = "OFF";
                    }
				}
                
                if (TeleportStatus) {
                    if (GetAsyncKeyState(SAVE_LOCATION_1)) {

                        OnePressTMR = clock();
                        UpdateOnNextRun = true;

                        positions[LOCATION_1][XCOORD] = GetCoordinate(hProcHandle,XCOORD);
                        positions[LOCATION_1][YCOORD] = GetCoordinate(hProcHandle,YCOORD);
                        positions[LOCATION_1][ZCOORD] = GetCoordinate(hProcHandle,ZCOORD);

                    } else if (GetAsyncKeyState(SAVE_LOCATION_2)) {
                        OnePressTMR = clock();
                        UpdateOnNextRun = true;
                        //changes the text to update on next display
                        positions[LOCATION_2][XCOORD] = GetCoordinate(hProcHandle,XCOORD);
                        positions[LOCATION_2][YCOORD] = GetCoordinate(hProcHandle,YCOORD);
                        positions[LOCATION_2][ZCOORD] = GetCoordinate(hProcHandle,ZCOORD);

                    } else if (GetAsyncKeyState(LOAD_LOCATION_1)) {
                        OnePressTMR = clock();
                        UpdateOnNextRun = true;
                        WriteCoordinates(hProcHandle, LOCATION_1);
                    } else if (GetAsyncKeyState(LOAD_LOCATION_2)) {
                        OnePressTMR = clock();
                        UpdateOnNextRun = true;
                        WriteCoordinates(hProcHandle, LOCATION_2);
                    }
                }
				
                if (ableToMove) {
                    UpdateOnNextRun = true;

					bool move=false;
                    iPressed=false;
                    jPressed=false;
                    kPressed=false;
                    lPressed=false;
                    uPressed=false;
                    oPressed=false;
					qPressed=false;
                    if (GetAsyncKeyState(I_KEY)) {
                        iPressed=true;
                    }
                    if (GetAsyncKeyState(J_KEY)) {
                        jPressed=true;
                    }
                    if (GetAsyncKeyState(K_KEY)) {
                        kPressed=true;
                    }
                    if (GetAsyncKeyState(L_KEY)) {
                        lPressed=true;
                    }
                    if (GetAsyncKeyState(U_KEY)) {
                        uPressed=true;
                    }
                    if (GetAsyncKeyState(O_KEY)) {
                        oPressed=true;
                    }
					if (GetAsyncKeyState(Q_KEY)) {
                        qPressed=true;
                    }

					float scale = 0.001;
					float vertscale = 3;
                    if (iPressed) {
						// Change vert/hori angles dependant on key pressed
						DWORD tempAngle = GetAngle(hProcHandle, 0);
						hangle = *((float *)&tempAngle);
						tempAngle = GetAngle(hProcHandle, 1);
						vangle = *((float *)&tempAngle);
						
						// Use trig to figure out angles
	
						float xdisplace = sin (hangle*PI/180);
						float zdisplace = cos (hangle*PI/180);
						float ydisplace = sin (vangle*PI/180);

						// Normalise vector + apply scaling
						float looklen = sqrtf(xdisplace*xdisplace + ydisplace*ydisplace + zdisplace*zdisplace);
						xdisplace = scale * (xdisplace/looklen);
						zdisplace = -1 * scale * (zdisplace/looklen);
						ydisplace = vertscale * scale * (ydisplace/looklen);

						// Set new X, Y, Z coords
                        float newCoordinate = addToCoordinate(GetCoordinate(hProcHandle, XCOORD), xdisplace);
                        WriteCoordinate(hProcHandle, XCOORD, newCoordinate);
                        float newCoordinate2 = addToCoordinate(GetCoordinate(hProcHandle, ZCOORD), zdisplace);
                        WriteCoordinate(hProcHandle, ZCOORD, newCoordinate2);
                        float newCoordinate3 = addToCoordinate(GetCoordinate(hProcHandle, YCOORD), ydisplace);
                        WriteCoordinate(hProcHandle, YCOORD, newCoordinate3);
                    }
                    if (jPressed) {
						// Change vert/hori angles dependant on key pressed
						DWORD tempAngle = GetAngle(hProcHandle, 0);
						hangle = fmod(*((float *)&tempAngle) + 270,360);
						// tempAngle = GetAngle(hProcHandle, 1);
						// vangle = *((float *)&tempAngle);
						
						// Use trig to figure out angles
						float xdisplace = sin (hangle*PI/180);
						float zdisplace = cos (hangle*PI/180);
						// float ydisplace = sin (vangle*PI/180);

						// Normalise vector + apply scaling
						float looklen = sqrtf(xdisplace*xdisplace + zdisplace*zdisplace);
						xdisplace = scale * (xdisplace/looklen);
						zdisplace = -1 * scale * (zdisplace/looklen);
						// ydisplace = vertscale * scale * (ydisplace/looklen);

						// Set new X, Y, Z coords
                        float newCoordinate = addToCoordinate(GetCoordinate(hProcHandle, XCOORD), xdisplace);
                        WriteCoordinate(hProcHandle, XCOORD, newCoordinate);
                        float newCoordinate2 = addToCoordinate(GetCoordinate(hProcHandle, ZCOORD), zdisplace);
                        WriteCoordinate(hProcHandle, ZCOORD, newCoordinate2);
                        // float newCoordinate3 = addToCoordinate(GetCoordinate(hProcHandle, YCOORD), ydisplace);
                        // WriteCoordinate(hProcHandle, YCOORD, newCoordinate3);
                    }
                    if (kPressed) {
						// Change vert/hori angles dependant on key pressed
						DWORD tempAngle = GetAngle(hProcHandle, 0);
						hangle = fmod(*((float *)&tempAngle)+180,360);
						tempAngle = GetAngle(hProcHandle, 1);
						vangle = *((float *)&tempAngle);
						
						// Use trig to figure out angles
						float xdisplace = sin (hangle*PI/180);
						float zdisplace = cos (hangle*PI/180);
						float ydisplace = sin (vangle*PI/180);

						// Normalise vector + apply scaling
						float looklen = sqrtf(xdisplace*xdisplace + ydisplace*ydisplace + zdisplace*zdisplace);
						xdisplace = scale * (xdisplace/looklen);
						zdisplace = -1 * scale * (zdisplace/looklen);
						ydisplace = -1 * vertscale * scale * (ydisplace/looklen);

						// Set new X, Y, Z coords
                        float newCoordinate = addToCoordinate(GetCoordinate(hProcHandle, XCOORD), xdisplace);
                        WriteCoordinate(hProcHandle, XCOORD, newCoordinate);
                        float newCoordinate2 = addToCoordinate(GetCoordinate(hProcHandle, ZCOORD), zdisplace);
                        WriteCoordinate(hProcHandle, ZCOORD, newCoordinate2);
                        float newCoordinate3 = addToCoordinate(GetCoordinate(hProcHandle, YCOORD), ydisplace);
                        WriteCoordinate(hProcHandle, YCOORD, newCoordinate3);
                    }
                    if (lPressed) {
						// Change vert/hori angles dependant on key pressed
						DWORD tempAngle = GetAngle(hProcHandle, 0);
						hangle = fmod(*((float *)&tempAngle) + 90,360);
						// tempAngle = GetAngle(hProcHandle, 1);
						// vangle = *((float *)&tempAngle) * -1;
						
						// Use trig to figure out angles
						float xdisplace = sin (hangle*PI/180);
						float zdisplace = cos (hangle*PI/180);
						// float ydisplace = sin (vangle*PI/180);

						// Normalise vector + apply scaling
						float looklen = sqrtf(xdisplace*xdisplace + zdisplace*zdisplace);
						xdisplace = scale * (xdisplace/looklen);
						zdisplace = -1 * scale * (zdisplace/looklen);
						// ydisplace = vertscale * scale * (ydisplace/looklen);

						// Set new X, Y, Z coords
                        float newCoordinate = addToCoordinate(GetCoordinate(hProcHandle, XCOORD), xdisplace);
                        WriteCoordinate(hProcHandle, XCOORD, newCoordinate);
                        float newCoordinate2 = addToCoordinate(GetCoordinate(hProcHandle, ZCOORD), zdisplace);
                        WriteCoordinate(hProcHandle, ZCOORD, newCoordinate2);
                        // float newCoordinate3 = addToCoordinate(GetCoordinate(hProcHandle, YCOORD), ydisplace);
                        // WriteCoordinate(hProcHandle, YCOORD, newCoordinate3);
                    }
					if (qPressed) {
						// Aimbot time
						int closestPlayer = getClosestPlayer(hProcHandle);
						if (closestPlayer != -1) {
							//std::cout << "Going to aim at player " << closestPlayer << std::endl;
							aimAtPlayer(hProcHandle,closestPlayer,false);
						} else {
							//std::cout << "Couldnt find a close player. Map must be empty" << std::endl;
						}
					}
                }
				if(ableToBlink){
					UpdateOnNextRun = true;

                    m1Pressed=false;
					if (GetAsyncKeyState(M1_KEY)) {
                        m1Pressed=true;
                    }

					if(m1Pressed){
						int closestPlayer = getClosestPlayer(hProcHandle);
						if (closestPlayer != -1) {
							//std::cout << "Going to aim at player " << closestPlayer << std::endl;
							aimAtPlayer(hProcHandle,closestPlayer,true);
						} else {
							//std::cout << "Couldnt find a close player. Map must be empty" << std::endl;
						}
					}
				}

            }
        }
    }

    //Close any handles once the program is over
    CloseHandle (hProcHandle);
    CloseHandle (hGameWindow);

    //Returns that action was completed successfuly
    return ERROR_SUCCESS;
}

//Handles Dynamic memory allocation
//Receives how high the pointer level is e.g. 4 levels and from that calculates the initial address
//the offset values and the memory addresses for those offsets
DWORD FindDmaAddy (int PointerLevel, HANDLE hProcHandle, DWORD Offsets[], DWORD BaseAddress) {
    //DECLARE BASE ADDRESS
    DWORD pointer = BaseAddress;             // Declare a pointer of DWORD
    //USED TO output the contents in the pointer
    DWORD pTemp;

    DWORD pointerAddr;
    for (int i = 0; i < PointerLevel; i ++) {
        if (i == 0) {
            ReadProcessMemory(hProcHandle, (LPCVOID)pointer, &pTemp, 4, NULL);
        }
        //add first offset to that address
        pointerAddr = pTemp + Offsets[i];   // Set p1 to content of p + offset

        //Read memory one more time and exit the loop
        ReadProcessMemory (hProcHandle, (LPCVOID)pointerAddr, &pTemp, 4, NULL);
    }
    return pointerAddr;
}

void WriteToMemory(HANDLE hProcHandle) {
    if (AmmoStatus) {
        DWORD AmmoAddressToWrite = FindDmaAddy(3, hProcHandle, AmmoOffsets, AmmoBaseAddress);
        WriteProcessMemory( hProcHandle, (BYTE*)AmmoAddressToWrite, &AmmoValue, sizeof(AmmoValue), NULL);
    }

    if (HealthStatus) {
        //because health address is only one pointer in we send only to FindDmaAddy
        DWORD HealthAddressToWrite = FindDmaAddy(1, hProcHandle, HealthOffsets, HealthBaseAddress);
        WriteProcessMemory( hProcHandle, (BYTE*)HealthAddressToWrite, &HealthValue, sizeof(HealthValue), NULL);
    }
}

DWORD GetCoordinate(HANDLE hProcHandle, int coordinate) {
    //std::cout << "ran getoordinate with" << coordinate << std::endl;
    DWORD LocationOffset[] = {LocationOffsets[coordinate]};
    //std::cout << "location offset "<<coordinate<<" is "<<LocationOffsets[coordinate]<<std::endl;
    DWORD coordinateLocation = (FindDmaAddy(1, hProcHandle, LocationOffset, LocationBaseAddress));
    DWORD coordinateValue;
    ReadProcessMemory (hProcHandle, (LPCVOID)coordinateLocation, &coordinateValue, 4, NULL);
    return coordinateValue;
}

DWORD GetPlayerCoordinate(HANDLE hProcHandle, DWORD playeraddress, int coordinate) {
    //std::cout << "ran getoordinate with" << coordinate << std::endl;
    //std::cout << "location offset "<<coordinate<<" is "<<LocationOffsets[coordinate]<<std::endl;
    DWORD coordinateLocation = playeraddress + LocationOffsets[coordinate];
	DWORD coordinateValue;
	ReadProcessMemory (hProcHandle, (LPCVOID)coordinateLocation, &coordinateValue, 4, NULL);
    return coordinateValue;
}

DWORD GetAngle(HANDLE hProcHandle, int direction) {
    //std::cout << "ran getoordinate with" << coordinate << std::endl;
    DWORD AngleOffset[] = {AngleOffsets[direction]};
    //std::cout << "location offset "<<coordinate<<" is "<<LocationOffsets[coordinate]<<std::endl;
    DWORD angleLocation = (FindDmaAddy(1, hProcHandle, AngleOffset, LocationBaseAddress));
    DWORD angleValue;
    ReadProcessMemory (hProcHandle, (LPCVOID)angleLocation, &angleValue, 4, NULL);
    return angleValue;
}

void SetAngle(HANDLE hProcHandle, int direction, float angle) {
    //std::cout << "ran getoordinate with" << coordinate << std::endl;
    DWORD AngleOffset[] = {AngleOffsets[direction]};
    //std::cout << "location offset "<<coordinate<<" is "<<LocationOffsets[coordinate]<<std::endl;
    DWORD angleLocation = (FindDmaAddy(1, hProcHandle, AngleOffset, LocationBaseAddress));
    
	// Write the angle to memory
	WriteProcessMemory (hProcHandle, (BYTE*)angleLocation, &angle, sizeof(angle), NULL);
}

void WriteCoordinates(HANDLE hProcHandle, int locationNumber) {
    //std::cout << "ran getoordinate with" << coordinate << std::endl;
    DWORD XLOC[] = {LocationOffsets[XCOORD]};
    DWORD YLOC[] = {LocationOffsets[YCOORD]};
    DWORD ZLOC[] = {LocationOffsets[ZCOORD]};
    //std::cout << "location offset "<<coordinate<<" is "<<LocationOffsets[coordinate]<<std::endl;
    DWORD addressX = (FindDmaAddy(1, hProcHandle, XLOC, LocationBaseAddress));
    DWORD addressY = (FindDmaAddy(1, hProcHandle, YLOC, LocationBaseAddress));
    DWORD addressZ = (FindDmaAddy(1, hProcHandle, ZLOC, LocationBaseAddress));

    WriteProcessMemory (hProcHandle, (BYTE*)addressX, &positions[locationNumber][XCOORD], sizeof(positions[locationNumber][XCOORD]), NULL);
    WriteProcessMemory (hProcHandle, (BYTE*)addressY, &positions[locationNumber][YCOORD], sizeof(positions[locationNumber][YCOORD]), NULL);
    WriteProcessMemory (hProcHandle, (BYTE*)addressZ, &positions[locationNumber][ZCOORD], sizeof(positions[locationNumber][ZCOORD]), NULL);
}

void WriteCoordinate (HANDLE hProcHandle, int coordinate, float valueToWrite) {
    DWORD Location[] = {LocationOffsets[coordinate]};
    DWORD address = (FindDmaAddy(1, hProcHandle, Location, LocationBaseAddress));
	//std::cout << "ACTUAL WRITING " << valueToWrite << std::endl;
    WriteProcessMemory (hProcHandle, (BYTE*)address, &valueToWrite, sizeof(valueToWrite), NULL);
}

float addToCoordinate(DWORD coordinate, float valueToAdd) {
    float coordinateF = *(float *)&coordinate;
    float newCoordinateF = coordinateF+valueToAdd;
    //std::cout << "old coordinate is " << coordinateF<< std::endl;
    //std::cout << "new coordinate is " << newCoordinateF << std::endl;
    return newCoordinateF;
}

void aimAtPlayer(HANDLE hProcHandle, int player, bool tele) {
	DWORD players = 0x004E4E08;
	DWORD playersArray;
	ReadProcessMemory (hProcHandle, (LPCVOID)players, &playersArray, 4, NULL);
	
	DWORD addressOfPlayerState = (playersArray+(0x4*player));
	DWORD playerState;
	ReadProcessMemory (hProcHandle, (LPCVOID)(addressOfPlayerState), &playerState, 4, NULL);
	
	// If the player actually exists
	if (playerState != 0) {
		DWORD xcoord = GetCoordinate(hProcHandle,XCOORD);
		DWORD ycoord = GetCoordinate(hProcHandle,YCOORD);
		DWORD zcoord = GetCoordinate(hProcHandle,ZCOORD);
		float myxcoord = *(float *)&xcoord;
		float myycoord = *(float *)&ycoord;
		float myzcoord = *(float *)&zcoord;

		xcoord = GetPlayerCoordinate(hProcHandle,playerState,XCOORD);
		ycoord = GetPlayerCoordinate(hProcHandle,playerState,YCOORD);
		zcoord = GetPlayerCoordinate(hProcHandle,playerState,ZCOORD);
		float hisxcoord = *(float *)&xcoord;
		float hisycoord = *(float *)&ycoord;
		float hiszcoord = *(float *)&zcoord;

		if(tele){
			// TODO: Doesnt actualyl tele to them
			// Could be syncro issue? extra cycles before mass teleporting?
			// Could be angle problem?
			//std::cout << "player " <<  player << " hisxcoord " << hisxcoord << " hisycoord " << hisycoord << " hiszcoord " << hiszcoord << std::endl;
			WriteCoordinate(hProcHandle, XCOORD, hisxcoord);
			WriteCoordinate(hProcHandle, YCOORD, hisycoord);
            WriteCoordinate(hProcHandle, ZCOORD, hiszcoord);
		}

		// TODO - delete this shit
		float xdisplace = hisxcoord-myxcoord;
		float ydisplace = hisycoord-myycoord;
		float zdisplace = hiszcoord-myzcoord;

		float looklen = sqrtf(xdisplace*xdisplace + ydisplace*ydisplace + zdisplace*zdisplace);
		
		xdisplace = (xdisplace/looklen);
		zdisplace = (zdisplace/looklen);
		ydisplace = (ydisplace/looklen);

		float temphangle = (180/PI) * atan2f(zdisplace,xdisplace) + 90;
		float tempvangle = (180/PI) * asin(ydisplace);
		//std::cout << "Me " << " myxcoord " << myxcoord << " myycoord " << myycoord << " myzcoord " << myzcoord << std::endl;
		// std::cout << "player " <<  player << " hisxcoord " << hisxcoord << " hisycoord " << hisycoord << " hiszcoord " << hiszcoord << std::endl;
		//std::cout << "Angle should be set to " <<  temphangle << " and " << tempvangle << std::endl;

		SetAngle(hProcHandle,0,temphangle);
		SetAngle(hProcHandle,1,tempvangle);

	}
}

// Returns the closest players number
DWORD getClosestPlayer(HANDLE hProcHandle) {
	DWORD players = 0x004E4E08;
	DWORD addressOfNumPlayers = 0x004E4E10;
	DWORD numPlayers;
	ReadProcessMemory (hProcHandle, (LPCVOID)addressOfNumPlayers, &numPlayers, 4, NULL);
	DWORD health = {0};
	
	DWORD playersArray;
	ReadProcessMemory (hProcHandle, (LPCVOID)players, &playersArray, 4, NULL);

	int closestPlayer = -1;
	DWORD closestDistance = 9999999;
	for (int player = 0; player < numPlayers && numPlayers < 15 && numPlayers > 0; player++) {
		DWORD addressOfPlayerState = (playersArray+(0x4*player));
		DWORD playerState;
		ReadProcessMemory (hProcHandle, (LPCVOID)(addressOfPlayerState), &playerState, 4, NULL);
		
		if (playerState != 0) {
			DWORD xcoord = GetCoordinate(hProcHandle,XCOORD);
			DWORD ycoord = GetCoordinate(hProcHandle,YCOORD);
			DWORD zcoord = GetCoordinate(hProcHandle,ZCOORD);
			float myxcoord = *(float *)&xcoord;
			float myycoord = *(float *)&ycoord;
			float myzcoord = *(float *)&zcoord;

			xcoord = GetPlayerCoordinate(hProcHandle,playerState,XCOORD);
			ycoord = GetPlayerCoordinate(hProcHandle,playerState,YCOORD);
			zcoord = GetPlayerCoordinate(hProcHandle,playerState,ZCOORD);
			float hisxcoord = *(float *)&xcoord;
			float hisycoord = *(float *)&ycoord;
			float hiszcoord = *(float *)&zcoord;

			float distancebetweenus = sqrtf((hisxcoord-myxcoord)*(hisxcoord-myxcoord) + (hisycoord-myycoord)*(hisycoord-myycoord) + (hiszcoord-myzcoord)*(hiszcoord-myzcoord));
			//std::cout << "player " <<  player << " hisxcoord " << hisxcoord << " hisycoord " << hisycoord << " hiszcoord " << hiszcoord << std::endl;
			//std::cout << "player " <<  player << "  distance = " << distancebetweenus << std::endl;
			if (distancebetweenus < closestDistance) {
				closestDistance = distancebetweenus;
				closestPlayer = player;
			}
		} else {
			//std::cout << "player " << player << " is not available" << std::endl;
		}
	}

	return closestPlayer;
}

void getPlayerHealth(HANDLE hProcHandle) {	
	DWORD players = 0x004E4E08;
	DWORD addressOfNumPlayers = 0x004E4E10;
	DWORD numPlayers;
	ReadProcessMemory (hProcHandle, (LPCVOID)addressOfNumPlayers, &numPlayers, 4, NULL);
	DWORD health = {0};
	
	DWORD playersArray;
	ReadProcessMemory (hProcHandle, (LPCVOID)players, &playersArray, 4, NULL);

	for (int player = 0; player < numPlayers && numPlayers < 15 && numPlayers > 0; player++) {
		DWORD addressOfPlayerState = (playersArray+(0x4*player));
		DWORD playerState;
		ReadProcessMemory (hProcHandle, (LPCVOID)(addressOfPlayerState), &playerState, 4, NULL);
		
		if (playerState != 0) {
			DWORD addyOfHealth = playerState+0xF4;
			ReadProcessMemory (hProcHandle, (LPCVOID)(addyOfHealth), &health, 4, NULL);
			if (health > 100) {
				//std::cout << "player " <<  player << " health: DEAD!"<< std::endl;
			} else {
				//std::cout << "player " <<  player << " health: " << health <<std::endl;
			}
		}
	}
}
