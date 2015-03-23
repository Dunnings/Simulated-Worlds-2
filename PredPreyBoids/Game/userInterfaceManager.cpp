#include "userInterfaceManager.h"
#include "helper.h"
#include "gamedata.h"




UserInterfaceManager::UserInterfaceManager(){
	addToDraw("Debug Menu", "Menu");
}

UserInterfaceManager::~UserInterfaceManager(){

}

void UserInterfaceManager::addToDraw(string s, string setting){
	UIElement* p = new UIElement;
	p->text = *Helper::charToWChar(s.c_str());
	POINT poi;
	poi.x = 5;
	poi.x += 5 * toDraw.size();
	poi.y = 5;
	p->location = poi;
	toDraw.push_back(p);
}

void UserInterfaceManager::Draw(DrawData* DD){

	//GameObject::Draw(DD);
}

void UserInterfaceManager::Tick(GameData* GD){
	addToDraw("Debug Menu", "Menu");


	if (!GD->prevMouse->rgbButtons[0] && GD->mouse->rgbButtons[0]){
		POINT loc;
		GetCursorPos(&loc);
		for (list<UIElement*>::iterator it = toDraw.begin(); it != toDraw.end(); it++){
			UIElement* currentElement = (*it);
			RECT r;
			r.top = currentElement->location.y;
			r.bottom = currentElement->location.y + 3.0f;
			r.left = currentElement->location.x;
			r.right = currentElement->location.y + 3.0f;

			if (PtInRect(&r, loc)){
				if (currentElement->setting == "total_boids"){
					//nothing
				}
			}
		}
	}
	toDraw.clear();
	GameObject::Tick(GD);
}