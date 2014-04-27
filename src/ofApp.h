#pragma once

#include "ofMain.h"
#include "TmxParser/Tmx.h"

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        void drawTile( int layerId, const Tmx::Layer *layer, int x, int y);
        void printDebugInfo( );
    
        Tmx::Map *map = new Tmx::Map();
        vector<ofImage> tileSheets;
    const Tmx::Tileset *tileset;
    
    int tileHeight;
    int tileWidth;
    int sheetWidth;
    int sheetHeight;
    int sheetSpacing;
    int sheetMargin;
};
