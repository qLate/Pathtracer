#pragma once

class Object;
class Graphical;

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
