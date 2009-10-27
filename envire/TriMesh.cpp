#include "Core.hpp"
#include "TriMesh.hpp"

#include <stdexcept>

using namespace envire;

const std::string TriMesh::className = "envire::TriMesh";

TriMesh::TriMesh()
{
}

TriMesh::TriMesh(Serialization& so)
    : CartesianMap(so)
{
    so.setClassName(className);
}

void TriMesh::serialize(Serialization& so)
{
    CartesianMap::serialize(so);
    so.setClassName(className);
}

TriMesh* TriMesh::clone() 
{
    return new TriMesh(*this);
}

void TriMesh::writeMap(const std::string& path)
{
    // TODO write to ply file
    throw std::runtime_error("not yet implemented");
}

void TriMesh::readMap(const std::string& path)
{
    // TODO read from ply file
    throw std::runtime_error("not yet implemented");
}