#include "gameobject.h"
#include <list>

enum UIMode
{
	UI_DEBUG,
	UI_ADD_NEW
};


using namespace std;
struct UIElement
{
	POINT location;
	string setting;
	wchar_t text;
};

class UserInterfaceManager : public GameObject{
public:
	UserInterfaceManager();
	~UserInterfaceManager();
	void toggleMode();
	void Draw(DrawData* DD);
	void Tick(GameData* GD);
	void addToDraw(string s, string setting);
	list<UIElement*> toDraw;
};