
#include <iostream> //for 'cout'
#include <fstream> //for writing to file
#include <math.h> //for math



int castAsInteger;

//color
int R;
int G;
int B;
float _R;
float _G;
float _B;
float savedR;
float savedG;
float savedB;

//some environment parameters
const float ambient = 0.1;
const float diffuse = 0.5;
const float FOV = 300;
const float phongValue = 0.8;

//camera parameters
const float camXDir = 0;
const float camYDir = 0;
const float camX = 40;
const float camY = 0;
const float camZ = -250;

//these mostly just hold values temporarily, mostly for optimization
float near;
float length;
float lightDist;
//this raytracer uses the phong lighting model
float phong = 0;

float renderDist;
float savedRayDirection;
float shade;
float specular;
float dist;
float dot;
float dotDir;

//vectors

//vector position
float centerX;
float centerY;
float centerZ;

//interseption vector
float interseptX;
float interseptY;
float interseptZ;

//light rays
float lightX;
float lightY;
float lightZ;
float lightx;
float lighty;
float lightz;

//surface normal
float normalX;
float normalY;
float normalZ;

//vector head
float rayX;
float rayY;
float rayZ;

//reflection vector
float reflectionX;
float reflectionY;
float reflectionZ;

//vector value holder
float sphereX;
float sphereY;
float sphereZ;


//set final pixel color
void setPenColorTo(float spctR, float spctG, float spctB) {
	R = floor(spctR);
	G = floor(spctG);
	B = floor(spctB);
}

//clamp values, transfer to _ R/G/B
void setRGBColorTo(float srgbctR, float srgbctG, float srgbctB) {
	_R = srgbctR * (!srgbctR > 255) + 255 * (srgbctR > 255);
	_G = srgbctG * (!srgbctG > 255) + 255 * (srgbctG > 255);
	_B = srgbctB * (!srgbctB > 255) + 255 * (srgbctB > 255);
}

//calc plane distance
void plane(float pNormalX, float pNormalY, float pNormalZ, float pDistance, float pSize, int pR1, int pG1, int pB1, int pR2, int pG2, int pB2) {
	dotDir = pNormalX * rayX + pNormalY * rayY + pNormalZ * rayZ;
	dot = pNormalX * centerX + pNormalY * centerY + pNormalZ * centerZ;
	dist = (dot + pDistance) / dotDir;
	if ((dotDir != 0) && (dist < 0) && ((0 - dist) < renderDist)) {
		renderDist = 0 - dist;
		//creates checkerboard pattern on the plane
		castAsInteger = (round((renderDist * rayX + camX) / pSize) + round((renderDist * rayZ + camZ) / pSize));
		if ((castAsInteger % 2) == 1) {
			setRGBColorTo(pR1, pG1, pB1);
		} else {
			setRGBColorTo(pR2, pG2, pB2);
		}
		
		//determines the surface normal of the plane
		//already normalized
		normalX = pNormalX;
		normalY = pNormalY;
		normalZ = pNormalZ;
	}
}

//calculate sphere distance
void sphere(float sX, float sY, float sZ, float sRad, int sR, int sG, int sB) {
	sphereX = sX - centerX;
	sphereY = sY - centerY;
	sphereZ = sZ - centerZ;
	dot = sphereX * rayX + sphereY * rayY + sphereZ * rayZ;
	length = sphereX * sphereX + sphereY * sphereY + sphereZ * sphereZ;
	if ((dot > 0) && (sRad * sRad > length - dot * dot)) {
		near = sqrt(sRad * sRad - (length - dot * dot));
		if ((length > sRad * sRad) && (renderDist > dot - near)) {
			renderDist = dot - near;
			setRGBColorTo(sR, sG, sB);
			//sets the surface normal of each point hit on the surface of the circle
			normalX = centerX + rayX * renderDist - sX;
			normalY = centerY + rayY * renderDist - sY;
			normalZ = centerZ + rayZ * renderDist - sZ;
			dist = sqrt(normalX * normalX + normalY * normalY + normalZ * normalZ); //normalize this vector
			normalX = normalX / dist;
			normalY = normalY / dist;
			normalZ = normalZ / dist;
		}
	}
}

//scene params
void calculateDistance() {
	sphere(-30, -20, -20, 50, 255, 0, 0);
	sphere(30, 20, 80, 100, 0, 255, 0);
	plane(0, 1, 0, 100, 50, 255, 255, 255, 200, 200, 200);
}

//light position, could be used to apply randomness or something
void setLightPosition(float slpLightX, float slpLightY, float slpLightZ) {
	lightX = slpLightX;
	lightY = slpLightY;
	lightZ = slpLightZ;
}

//cast out rays, calculcate colors, etc.
void raytracePixel(float rpX, float rpY, float rpXDir, float rpYDir, float rpCamX, float rpCamY, float rpCamZ) {
	centerX = rpCamX;
	centerY = rpCamY;
	centerZ = rpCamZ;
	rayX = rpX;
	rayY = rpY;
	rayZ = FOV;
	dist = sqrt(rayX * rayX + rayY * rayY + rayZ * rayZ);
	rayX = rayX / dist;
	rayY = rayY / dist;
	rayZ = rayZ / dist;
	savedRayDirection = rayZ;
	rayZ = cos(rpYDir) * rayZ - sin(rpYDir) * rayY;
	rayY = sin(rpYDir) * savedRayDirection + cos(rpYDir) * rayY;
	savedRayDirection = rayZ;
	rayZ = cos(rpXDir) * rayZ - sin(rpXDir) * rayX;
	rayY = sin(rpXDir) * savedRayDirection + cos(rpXDir) * rayX;
	renderDist = 10000;
	calculateDistance(); //changes "renderDist"
	if (10000 > renderDist) {
		interseptX = centerX + rayX * renderDist;
		interseptY = centerY + rayY * renderDist;
		interseptZ = centerZ + rayZ * renderDist;
		lightx = lightX - interseptX;
		lighty = lightY - interseptY;
		lightz = lightZ - interseptZ;
		dist = sqrt(lightx * lightx + lighty * lighty + lightz * lightz);
		lightx = lightx / dist;
		lighty = lighty / dist;
		lightz = lightz / dist;
		dot = normalX * lightx + normalY * lighty + normalZ * lightz;
		reflectionX = 2 * normalX * dot - rayX;
		reflectionY = 2 * normalY * dot - rayY;
		reflectionZ = 2 * normalZ * dot - rayZ;
		dot = 0 - (lightx * reflectionX + lighty * reflectionY + lightz * reflectionZ);
		if (dot > 0) {
			phong = dot * dot * dot * dot * dot * dot * dot; //determines the specularity of a pixel.
		} else {
			phong = 0;
		}
		lightDist = dist;
		renderDist = dist;
		rayX = lightx;
		rayY = lighty;
		rayZ = lightz;
		centerX = interseptX;
		centerY = interseptY;
		centerZ = interseptZ;
		//these lines 211 - 215 are essentially useless, they could be remove. (for testing purposes)
		savedR = _R;
		savedG = _G;
		savedB = _B;
		calculateDistance();
		setRGBColorTo(savedR, savedG, savedB);
		if (lightDist == renderDist) {
			specular = phong * phongValue;
			shade = ambient + diffuse * shade;
			if (shade > 1) {
				shade = 1;
			}
		} else {
			specular = 0;
			shade = ambient;
		}
		//apply shading (if not occluded from light source)
		setRGBColorTo(255 * specular + _R * shade, 255 * specular + _G * shade, 255 * specular + _B * shade);
		setPenColorTo(_R, _G, _B);
	}
	else {
		setPenColorTo(0, 0, 0);
	}
}




int main () 
{

	const int image_width = 480; //width
	const int image_height = 360; //height

	std::cout << "DIMENSIONS: " << image_width << " X " << image_height << "\n\n";
	
	//create file
	std::ofstream fout("CPP_RCA_Output_007.ppm"); //"fout" stands for "file-out", totally arbitrary
	if (fout.fail()) return -1; //if unable to create file, return error

	fout << "P3\n" << image_width << " " << image_height << "\n255\n"; //write to file. "fout" refers to the file written to

	setLightPosition(80, 150, -80);

	//these loops were from the "raytracing in one weekend" tutorial
	for (int j = image_height - 1; j >= 0; --j)
	{

		std::cerr << "\rRemaining: " << j << " " << std::flush; //how many lines are left (also from tutorial)

		for (int i = 0; i < image_width; ++i)
		{

			raytracePixel(i - image_width / 2, j - image_height / 2, camXDir, camYDir, camX, camY, camZ);

			auto r = R;
			auto g = G;
			auto b = B;

			int ir = static_cast<int>(255.999 * r);
			int ig = static_cast<int>(255.999 * g);
			int ib = static_cast<int>(255.999 * b);

			fout << ir << " " << ig << " " << ib << "\n"; //write colors to file
		}
	}

	std::cerr << "\n\nDONE.\n";

	fout.close(); //end file

	return 0;
}
