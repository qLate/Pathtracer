#pragma once

class Disk;
class PointLight;
class Light;
class Mesh;
class Sphere;
class Graphical;
class Object;

class ObjectInspectorDrawer
{
public:
	static void draw(Object* target);
};

class GraphicalInspectorDrawer
{
public:
	static void draw(Graphical* target);
};

class SphereInspectorDrawer
{
public:
	static void draw(Sphere* target);
};

class DiskInspectorDrawer
{
public:
	static void draw(Disk* target);
};

class MeshInspectorDrawer
{
public:
	static void draw(Mesh* target);
};

class LightInspectorDrawer
{
public:
	static void draw(Light* target);
};

class PointLightInspectorDrawer
{
public:
	static void draw(PointLight* target);
};