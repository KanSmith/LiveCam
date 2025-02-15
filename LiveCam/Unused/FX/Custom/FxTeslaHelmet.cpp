#include <fx/FxTeslaHelmet.h>

#include <stdlib.h>

FxTeslaHelmet::FxTeslaHelmet()
{
	auto helmetShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/backClippingSimpleVertexShader.txt", "./assets/shaders/fragment/backClippingPhongFragmentShader.txt");

	helmet = make_shared<FxWidget3D>();
	
	helmet->modelPath = "./assets/fx/teslaHelmet/TeslaCoil_Hat_NoBolts.obj";
	helmet->modelShift = { 0, 1.07f, 0 };
	helmet->modelScale = 1.85f;

	helmet->extraRotateMatrix(0, 0) = -1;
	helmet->extraRotateMatrix(2, 2) = -1;

	helmet->texturesPaths.fill( {"./assets/fx/teslaHelmet/TeslaHelmet_ColorUV2.jpg" });
	helmet->name = "helmet";

	helmetRenderParams = make_shared<ObjectRenderParams>();
	helmetRenderParams->cameraPos = { 0, 0, 0, 1 };
	helmetRenderParams->ambientLight = { 1, 1, 1 };
	helmetRenderParams->specularPower = 7;
	helmetRenderParams->blend = true;
	helmetRenderParams->shader = helmetShader;

	auto &addUniforms = helmetRenderParams->additionalUniforms;
	addUniforms["ZClip"] = TUniform1f(0);
	addUniforms["YClip"] = TUniform1f(0);
	addUniforms["ZRotation"] = TUniform3f(Eigen::Vector3f(0, 0, 0));

	helmet->objectRenderParams.fill( { helmetRenderParams });

	models.push_back(helmet);

	boltSprite.animationPath = "./assets/fx/teslaHelmet/bolts/";
	boltSprite.animationIndex = 0;
	boltSprite.shader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/spriteVertexShader.txt", "./assets/shaders/fragment/spriteFragmentShader.txt");
	
	srand(time(NULL));
	for (int i = 0; i < ALPHA_COUNT; ++i)
	{
		for (int p = 0; p < PHI_COUNT; ++p)
		{
			animationStart[i][p] = rand() % MAX_RELOADING_TIME + 1;
			animationIndex[i][p] = -1;
		}
	}
}

FxTeslaHelmet::~FxTeslaHelmet()
{

}

void FxTeslaHelmet::load()
{
	FX::load();

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		auto &addUniforms = helmet->objectRenderParams[i][0]->additionalUniforms;
		addUniforms["ZClip"] = TUniform1f(helmet->bmin[2] + (helmet->bmax[2] - helmet->bmin[2]) * 0.5f);
		addUniforms["YClip"] = TUniform1f(helmet->bmin[1] + (helmet->bmax[1] - helmet->bmin[1]) * 0.1f);
	}

	float scale = (helmet->bmax[0] - helmet->bmin[0]) * 1.5f;
	boltSprite.animationScale = { scale, scale };
	boltSprite.animationOffset = { helmet->bmin[0], helmet->bmax[1], 0 };

	boltSprite.load();
}

void FxTeslaHelmet::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	FX::transform(face, externalRenderParams);
	

	auto ZRotation = helmet->renderParams.rotationMatrix.block<1, 3>(2, 0).data();
	helmet->objectRenderParams[face.pointId][0]->additionalUniforms["ZRotation"] =
		TUniform3f(Eigen::Vector3f(ZRotation[0], ZRotation[1], ZRotation[2]));

	float factor = 0;
	
	for (int i = 0; i < ALPHA_COUNT; ++i)
	{
		for (int p = 0; p < PHI_COUNT; ++p)
		{
			factor += animationIndex[i][p] == -1 ? 0 : animationIndex[i][p] * 3.f  / (boltSprite.animationLength - 1.f) / PHI_COUNT / ALPHA_COUNT;

			if (animationStart[i][p] > 0)
			{
				--animationStart[i][p];
			}
			else
			{
				animationIndex[i][p] += 3;

				if (animationIndex[i][p] >= boltSprite.animationLength)
				{
					animationIndex[i][p] = -1;
					animationStart[i][p] = rand() % MAX_RELOADING_TIME + 1;
				}
			}
		}
	}

 	helmetRenderParams->lightPos = helmet->renderParams.modelView * Vector4f(0, 19.82f, 0, 0);
 	helmetRenderParams->diffuseLight = { 0.0706f * factor, 0.8589f * factor, 0.9725f * factor };
 	helmetRenderParams->specularLight = { 0.0353f * factor, 0.4295f * factor, 0.4862f * factor };
}

void FxTeslaHelmet::draw(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	FX::draw(face, externalRenderParams);

	glDisable(GL_DEPTH_TEST);
	

	double phi_delta = M_PI_2 / (PHI_COUNT + 1);
	double phi = -M_PI_4 + phi_delta;

	for (int p = 0; p < PHI_COUNT; ++p, phi += phi_delta)
	{
		Matrix4d rotationZ;
		rotationZ = Affine3d(AngleAxisd(phi, Vector3d(0, 0, 1))).matrix();

		double alpha_delta = M_PI / ALPHA_COUNT;
		double alpha = 0;

		for (int i = 0; i < ALPHA_COUNT; ++i, alpha += alpha_delta)
		{
			Matrix4d rotationY;
			rotationY = rotationZ * Affine3d(AngleAxisd(alpha, Vector3d(0, 1, 0))).matrix();

			Matrix4f externalModelMatrix = helmet->renderParams.modelView * rotationY.cast<float>();

			boltSprite.animationIndex = animationIndex[i][p];
			boltSprite.draw(externalModelMatrix, helmet->renderParams.projection);
		}
	}
}

void FxTeslaHelmet::unload()
{
	FX::unload();

	boltSprite.unload();
}