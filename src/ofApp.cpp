#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(100);
    ofSetColor(255);
    
    //Load TMX file
    ofBuffer buffer = ofBufferFromFile("example.tmx");
    map->ParseText(buffer.getText());

    //Check error
    if (map->HasError()) {
		printf("error code: %d\n", map->GetErrorCode());
		printf("error text: %s\n", map->GetErrorText().c_str());
		system("PAUSE");
        
		return map->GetErrorCode();
	}
    
    //Debug info
    printDebugInfo();
    
    //Load images
    for (int i = 0; i < map->GetNumTilesets(); ++i) {
        //tileSheet.loadImage("tmw_desert_spacing.png");
        tileSheets.push_back(ofImage());
        (tileSheets.back()).loadImage( map->GetTileset(i)->GetImage()->GetSource().c_str() );
    }
    
    
}


//--------------------------------------------------------------
void ofApp::update(){
  }

//--------------------------------------------------------------
void ofApp::draw()
{
    //drawTile( 1, 4, 8 );
    //for( auto iter = tileset->GetTiles().begin(); iter != tileset->GetTiles().end(); iter++ )
    // Iterate through the layers.
	for (int i = 0; i < map->GetNumLayers(); ++i) {
    
        const Tmx::Layer *layer = map->GetLayer(i);
        
		for (int y = 0; y < layer->GetHeight(); ++y) {
			for (int x = 0; x < layer->GetWidth(); ++x) {
                drawTile( i, layer, x, y );
			}
		}
	}
    
    // Iterate through all of the object groups.
	for (int i = 0; i < map->GetNumObjectGroups(); ++i) {
		
		// Get an object group.
		const Tmx::ObjectGroup *objectGroup = map->GetObjectGroup(i);
        
		// Iterate through all objects in the object group.
		for (int j = 0; j < objectGroup->GetNumObjects(); ++j) {
			// Get an object.
			const Tmx::Object *object = objectGroup->GetObject(j);
            
			// Print information about the object.
			printf("Object Name: %s\n", object->GetName().c_str());
			printf("Object Position: (%03d, %03d)\n", object->GetX(), object->GetY());
			printf("Object Size: (%03d, %03d)\n", object->GetWidth(), object->GetHeight());
            
            ofPushMatrix();
            ofTranslate(object->GetX(), object->GetY());
            
            ofPushStyle();
            ofNoFill();
            ofSetColor(255,0,0);
            
            ofSetPolyMode(OF_POLY_WINDING_ODD);	// this is the normal mode

			// Print Polygon points.
			const Tmx::Polygon *polygon = object->GetPolygon();
			if (polygon != 0) {
                ofBeginShape();
				for (int i = 0; i < polygon->GetNumPoints(); i++) {
					const Tmx::Point &point = polygon->GetPoint(i);
                    ofVertex(point.x, point.y);
				}
                ofEndShape();

			}
            
			// Print Polyline points.
			const Tmx::Polyline *polyline = object->GetPolyline();
			if (polyline != 0) {
                ofBeginShape();
				for (int i = 0; i < polyline->GetNumPoints(); i++) {
					const Tmx::Point &point = polyline->GetPoint(i);
                    ofVertex(point.x, point.y);
				}
                ofEndShape();
			}
            
            ofPopStyle();
            ofPopMatrix();
		}
	}
    

}


void ofApp::drawTile( int layerId, const Tmx::Layer *layer, int x, int y) {
    
    int tileId = layer->GetTileId(x, y);
    
    //In ideal case tile 0 would mean transparent. Because no way to distinguish between empty and 0 afaik.
    //However in the example .tmx this is not the case
    //For now I only draw tile with id 0 on bottom layer, on other layers this tile will not render
    if( layerId > 0 && tileId == 0 ) {
        return;
    }
    
    //Get the right tileset and tilesheet
    int tilesetIndex = layer->GetTileTilesetIndex(x, y);
    const Tmx::Tileset * tileset = map->GetTileset(tilesetIndex);
    ofImage tileSheet = tileSheets[ tilesetIndex ];
    
    //determine what area from the tilesheet we need to draw
    int cols = tileSheet.getWidth() / tileset->GetTileWidth();
    int col = tileId % cols;
    int row = tileId / cols;
    int tileW = tileset->GetTileWidth();
    int tileH = tileset->GetTileHeight();
    int margin = tileset->GetMargin();
    int spacing = tileset->GetSpacing();
    int sx = margin + col * ( tileW + spacing );
    int sy = margin + row * ( tileH + spacing );
    
    //move to right position
    ofPushMatrix();
    ofTranslate( x * tileW, y * tileH );
    
    //do transforms for flipped tiles
    ofTranslate( tileW / 2, tileH / 2 );
    if (layer->IsTileFlippedHorizontally(x, y)){
        ofScale( -1, 1 );
    }
    if (layer->IsTileFlippedVertically(x, y)){
        ofScale(1,-1);
    }
    if (layer->IsTileFlippedDiagonally(x, y)){
        ofScale(-1,-1);
    }
    ofTranslate( -tileW / 2, -tileH / 2 );
    
    //draw it!
    tileSheet.drawSubsection( 0, 0, 32, 32, sx, sy);
    ofPopMatrix();
    
}
void ofApp::printDebugInfo( ) {
    //
    // Straight from TmxParser example
    //
    
	// Iterate through the tilesets.
	for (int i = 0; i < map->GetNumTilesets(); ++i) {
		printf("                                    \n");
		printf("====================================\n");
		printf("Tileset : %02d\n", i);
		printf("====================================\n");
        
		// Get a tileset.
		const Tmx::Tileset *tileset = map->GetTileset(i);
        
		// Print tileset information.
		printf("Name: %s\n", tileset->GetName().c_str());
		printf("Margin: %d\n", tileset->GetMargin());
		printf("Spacing: %d\n", tileset->GetSpacing());
		printf("Image Width: %d\n", tileset->GetImage()->GetWidth());
		printf("Image Height: %d\n", tileset->GetImage()->GetHeight());
		printf("Image Source: %s\n", tileset->GetImage()->GetSource().c_str());
		printf("Transparent Color (hex): %s\n", tileset->GetImage()->GetTransparentColor().c_str());
        
		if (tileset->GetTiles().size() > 0) {
			// Get a tile from the tileset.
			const Tmx::Tile *tile = *(tileset->GetTiles().begin());
            
			// Print the properties of a tile.
			std::map< std::string, std::string > list = tile->GetProperties().GetList();
			std::map< std::string, std::string >::iterator iter;
			for (iter = list.begin(); iter != list.end(); ++iter) {
				printf("%s = %s\n", iter->first.c_str(), iter->second.c_str());
			}
		}
	}
    
	// Iterate through the layers.
	for (int i = 0; i < map->GetNumLayers(); ++i) {
		printf("                                    \n");
		printf("====================================\n");
		printf("Layer : %02d/%s \n", i, map->GetLayer(i)->GetName().c_str());
		printf("====================================\n");
        
		// Get a layer.
		const Tmx::Layer *layer = map->GetLayer(i);
        
		for (int y = 0; y < layer->GetHeight(); ++y) {
			for (int x = 0; x < layer->GetWidth(); ++x) {
				// Get the tile's id.
				printf("%03d", layer->GetTileId(x, y));
                
				// Find a tileset for that id.
				const Tmx::Tileset *tileset = map->FindTileset(layer->GetTileId(x, y));
				if (layer->IsTileFlippedHorizontally(x, y)){
					printf("h");
				}else{
					printf(" ");
				}
				if (layer->IsTileFlippedVertically(x, y)){
					printf("v");
				}else{
					printf(" ");
				}
				if (layer->IsTileFlippedDiagonally(x, y)){
					printf("d ");
				} else {
					printf("  ");
				}
			}
            
			printf("\n");
		}
	}
    
	printf("\n\n");
    
	// Iterate through all of the object groups.
	for (int i = 0; i < map->GetNumObjectGroups(); ++i) {
		printf("                                    \n");
		printf("====================================\n");
		printf("Object group : %02d\n", i);
		printf("====================================\n");
        
		// Get an object group.
		const Tmx::ObjectGroup *objectGroup = map->GetObjectGroup(i);
        
		// Iterate through all objects in the object group.
		for (int j = 0; j < objectGroup->GetNumObjects(); ++j) {
			// Get an object.
			const Tmx::Object *object = objectGroup->GetObject(j);
            
			// Print information about the object.
			printf("Object Name: %s\n", object->GetName().c_str());
			printf("Object Position: (%03d, %03d)\n", object->GetX(), object->GetY());
			printf("Object Size: (%03d, %03d)\n", object->GetWidth(), object->GetHeight());
            
			// Print Polygon points.
			const Tmx::Polygon *polygon = object->GetPolygon();
			if (polygon != 0) {
				for (int i = 0; i < polygon->GetNumPoints(); i++) {
					const Tmx::Point &point = polygon->GetPoint(i);
					printf("Object Polygon: Point %d: (%d, %d)\n", i, point.x, point.y);
				}
			}
            
			// Print Polyline points.
			const Tmx::Polyline *polyline = object->GetPolyline();
			if (polyline != 0) {
				for (int i = 0; i < polyline->GetNumPoints(); i++) {
					const Tmx::Point &point = polyline->GetPoint(i);
					printf("Object Polyline: Point %d: (%d, %d)\n", i, point.x, point.y);
				}
			}
		}
	}
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}