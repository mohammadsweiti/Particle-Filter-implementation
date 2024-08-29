/****************+*******************

					group name :
		   Mohammad  Omar  sweity,ID  211098
		   Omar Ghandi Manassrah,ID  237014

*******************************/
/*
*  the csv file address in main function  as its location in our computer , please change it to the address in your computer 
 */


#include <iostream>
#include <random>
#include<cmath>
#include<fstream>
#include <sstream>
#include<string>

using namespace std;
const int numberOfParticle = 100;
int Env[80][80];// as chess board 
struct particles {
	float x, y, theta;
	float w;
};
void intEnv(int Env[80][80])//make the Env like chess 
{
	int s, w, z;
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			if (j != 0)//we put these conditions to make environment like Chess .,0 black,1 white
			{
				if (Env[i * 10][(j * 10) - 10])
				{
					z = 0;
				}
				else
				{
					z = 1;
				}
			}
			else if (i != 0)
			{
				if (Env[10 * i - 10][j * 10])
				{
					z = 0;
				}
				else
				{
					z = 1;
				}
			}
			else
			{
				z = 0;
			}
			for (int k = 0; k < 10; k++)
			{
				for (int l = 0; l < 10; l++)
				{
					w = l + (10 * j);
					s = k + 10 * i;
					Env[s][w] = z;
				}
			}
		}


}
void  normalize(particles S[])//To make the  Sum  of the particle = 1 
{
	 
	double sum = 0;
	for (int i = 0;i < numberOfParticle;i++)
		sum += S[i].w;

	for (int i = 0;i < numberOfParticle;i++)
	{
		S[i].w = S[i].w / static_cast<float>(sum);
	}
	
}
void  initialize(particles S[])
{
	srand(time(nullptr));// To get random number
	for (int i = 0;i < numberOfParticle;i++)
	{
		S[i].x = rand() % 80;// the enviroment is take that 80 *80 
		S[i].y = rand() % 80;
		S[i].theta = rand() % 360;
		S[i].w = 0.01;
	}

}
int sample(particles s[])
{
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<double> dis(0.0, 1.0);
	float ran = dis(gen); // this code is to get random number between 0 and 1 
	//float ran = (rand() % 10000)/10000;
	
	float cdf_weight = 0;
	for (int i = 0; i < 100; ++i)
	{
		cdf_weight += s[i].w; //Cumulative Distribution Function 
		if (ran <= cdf_weight) {
		//cout << endl << i <<" "<< cdf_weight << endl;
			return i;
			
		}

	}
	return 0;//the code will never reach this but we put  this to ignore compiler (syntax ) eror
}
void   update(particles s[], int z)
{ 
	int loc, m, c;
	for (int i = 0; i < numberOfParticle; i++)
	{
		m = s[i].x;
		c = s[i].y;
		loc = Env[m][c];
		if (z == 1 && loc == 1) // p(bright | bright )
		{
			s[i].w = 0.8;
		}
		else if (z == 1 && loc == 0)//p(bright  | dark)
		{
			s[i].w = 0.1;
		}
		else if (z == 0 && loc == 1)// p(dark | bright )
		{
			s[i].w = 0.2;
		}
		else if (z == 0 && loc == 0)// p(dark | dark )
		{
			s[i].w = 0.9;
		}
		
	}
}
void ParticleFilter(particles S[], int  Z, float    U)//U postive forward , U negative rotate 
{
	particles Snew[100];
	int i;
	for (int j = 0;j < numberOfParticle; j++)
	{
		i = sample(S);//sampling with replcament with cdf 
		if (U >= 0) {// Move forward: update x and y
			S[i].x = S[i].x + U * cos((S[i].theta * 3.14) / 180); // To move with direction of the robot 
			S[i].y = S[i].y + U * sin((S[i].theta * 3.14) / 180);// if the theta are 0 , or pi will  y will be the same but if theta = pi /2 or 2pi/3 x will n't  change 
			if (0 > S[i].x || S[i].x > 80) // if the particle move out of the Enviroment
			{
				S[i].x = rand() % 80;
			}
			if (0 > S[i].y || S[i].y > 80)
			{
				S[i].y = rand() % 80;
			}
		}
		else // Rotate only update theta
		{

			S[i].theta +=( - 1 * U); // Negative command for rotation
			if (S[i].theta < 0) {
				S[i].theta += 360;
			}
			else if (S[i].theta >= 360) {
				S[i].theta -= 360;
			}
		}
		//cout << S[i].x << "  " << S[i].y << endl;
		update(S, Z);
		
		Snew[j] = S[i];
	}
	normalize(Snew);
	for (int i = 0; i < numberOfParticle; i++)//return the value to the orginal array 
	{
		S[i] = Snew[i];
	}

}
void readCSVLine(const string& filename, int lineNumber, float& x, float& y, float& displacement, float& angle) {
	ifstream file(filename);
	string line;

	for (int i = 1; i <= lineNumber; ++i) {
		if (!getline(file, line)) {
			cout << "out of scope\n";
			return;
		}
		if (i == lineNumber) {
			istringstream iss(line);
			string value;
			int count = 0;
			while (getline(iss, value, ',')) {
				count++;
				switch (count) {
				case 1: x = stof(value); break;
				case 2: y = stof(value); break;
				case 3: displacement = stof(value); break;
				case 4: angle = stof(value); break;
				}
			}
			break;
		}
	}
	file.close();
}
void report(particles S[numberOfParticle], float robotX, float robotY)
{
	float meanParticleX = 0, meanParticleY = 0, varParticleX = 0, varParticleY = 0;
	float sum = 0;
	for (int i = 0;i < numberOfParticle;i++)
	{
		meanParticleX += S[i].x;
		meanParticleY += S[i].y;
	}
	meanParticleX /= float(numberOfParticle);
	meanParticleY /= float(numberOfParticle);

	for (int i = 0; i < numberOfParticle; i++)
	{
		varParticleX += pow(S[i].x - meanParticleX, 2);
		varParticleY += pow(S[i].y - meanParticleY, 2);
	}

	varParticleX /= numberOfParticle;
	varParticleY /= numberOfParticle;

	cout << robotX << " " << robotY << " " << meanParticleX << " " << meanParticleY << " " << varParticleX << " " << varParticleY << "\n";
}

int main()
{
	intEnv(Env);
	particles S[numberOfParticle];
	initialize(S);
	int z;
	for (int i = 0; i <= 500; i++)
	{
		float robotX, robotY, displacement, angle;
		readCSVLine("C:\\Users\\ttc\\Documents\\MohdSweity\\IS\\robot.csv", i + 2, robotX, robotY, displacement, angle);//the file address  is as in our device 
		cout << i << " " << robotX << " " << robotY << " " << displacement << " " << angle << endl;
		int x = robotX;
		int y = robotY;
		z = Env[x][y];//to return the sensor if the it in white or dark 
		ParticleFilter(S, z, -1 * angle); //Z: robot sensor take 0 or 1 , 0 : dark , 1 : Bright ,  U:command if postive move forward , negative rotate 
		ParticleFilter(S, z, displacement);
		report(S, robotX, robotY);
	}

	return 0;
}

