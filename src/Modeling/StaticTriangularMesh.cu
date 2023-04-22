#include "StaticTriangularMesh.h"

#include "Topology/TriangleSet.h"
#include <iostream>
#include <sys/stat.h>


namespace dyno
{
	IMPLEMENT_TCLASS(StaticTriangularMesh, TDataType)

	template<typename TDataType>
	StaticTriangularMesh<TDataType>::StaticTriangularMesh()
		: Node()
	{
		auto triSet = std::make_shared<TriangleSet<TDataType>>();
		this->stateTopology()->setDataPtr(triSet);

		this->outTriangleSet()->setDataPtr(triSet);
	}

	template<typename TDataType>
	void StaticTriangularMesh<TDataType>::resetStates()
	{
		auto triSet = TypeInfo::cast<TriangleSet<TDataType>>(this->stateTopology()->getDataPtr());
		
		if (this->varFileName()->getDataPtr()->string() == "")
			return;

		triSet->loadObjFile(this->varFileName()->getDataPtr()->string());

		triSet->scale(this->varScale()->getData());
		triSet->translate(this->varLocation()->getData());
		triSet->rotate(this->varRotation()->getData() * PI / 180);

		Node::resetStates();

		initPos.resize(triSet->getPoints().size());
		initPos.assign(triSet->getPoints());
		center = this->varCenter()->getData();
		centerInit = center;

	}

	template <typename Coord, typename Matrix>
	__global__ void K_InitKernelFunctionMesh(
		DArray<Coord> posArr,
		DArray<Coord> posInit,
		Coord center,
		Coord centerInit,
		Matrix rotation
	)
	{
		int pId = threadIdx.x + (blockIdx.x * blockDim.x);
		if (pId >= posArr.size())
			return;
		Coord pos;
		pos = posInit[pId] - centerInit;
		pos = rotation * pos;
		posArr[pId] = pos + center;

	}


	template<typename TDataType>
	void StaticTriangularMesh<TDataType>::updateStates()
	{


		auto triSet = TypeInfo::cast<TriangleSet<TDataType>>(this->stateTopology()->getDataPtr());

		if (this->varSequence()->getData() == true)
		{


		std::string filename = this->varFileName()->getDataPtr()->string();
		int num_ = filename.rfind("_");

		filename.replace(num_+1, filename.length()-4-(num_+1), std::to_string(this->stateFrameNumber()->getData()));


		struct stat buffer;
		bool isvaid = (stat(filename.c_str(), &buffer) == 0);

			if (isvaid)
			{
				triSet->loadObjFile(filename);

				triSet->scale(this->varScale()->getData());
				triSet->translate(this->varLocation()->getData());
				triSet->rotate(this->varRotation()->getData() * PI / 180);

				initPos.resize(triSet->getPoints().size());
				initPos.assign(triSet->getPoints());
				center = this->varCenter()->getData();
				centerInit = center;
			}


		}


		Coord velocity = this->varVelocity()->getData();
		Coord angularVelocity = this->varAngularVelocity()->getData();

		//printf("velocity = %.10lf %.10lf %.10lf\n", velocity[0], velocity[1], velocity[2]);

		Real dt = 0.001f;
		rotQuat = rotQuat.normalize();
		rotQuat += dt * 0.5f *
			Quat<Real>(angularVelocity[0], angularVelocity[1], angularVelocity[2], 0.0)*(rotQuat);

		rotQuat = rotQuat.normalize();
		rotMat = rotQuat.toMatrix3x3();

		center += velocity * dt;

		cuExecute(triSet->getPoints().size(),
			K_InitKernelFunctionMesh,
			triSet->getPoints(),
			initPos,
			center,
			centerInit,
			rotMat
			);

	}

	DEFINE_CLASS(StaticTriangularMesh);
}