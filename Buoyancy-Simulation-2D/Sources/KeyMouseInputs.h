#pragma once



//change to command pattern

int counterI = 0;	//counterI ahora no hace nada, pero voy a dejar la logic para cuando testee agujeros
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	AllPointers* allpointers = static_cast<AllPointers*>(glfwGetWindowUserPointer(window));
	NewPolygons* newPolygon = allpointers->newPolygon;
	Polygons* polygon = allpointers->polygon;
	BinariesManager* binariesManager = allpointers->binariesManager;

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		//changePositions(polygonPositions);
	}

	if (key == GLFW_KEY_P && action == GLFW_PRESS && continueRunning)
	{
		continueRunning = false;
	}
	else if (key == GLFW_KEY_P && action == GLFW_PRESS && !continueRunning)
	{
		continueRunning = true;
	}


	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		//glfwSetWindowShouldClose(window, GLFW_TRUE);


		if (binariesManager->currentProgramType == 1)
		{
			polygon->clear();
			polygon->addSet(binariesManager->readModel());

		}
		else if (binariesManager->currentProgramType == 0)
		{
			//newPolygon->clear();
			//newPolygon->addSet({ cursorX,cursorY });
		}
		counterI = 0;
	}
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{


		if (counterI < polygon->sortedPoints.size())
		{
			polygon->sweepTriangulation(/*counterI*/);
			counterI++;


		}

	}
	//SPACE
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{

		for (size_t i = 0; i < polygon->sortedPoints.size(); ++i)
		{
			polygon->sweepTriangulation();

		}
	}
	//CTRL+N
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS &&
		key == GLFW_KEY_N && action == GLFW_PRESS) {

		binariesManager->writeConfig();

		if (binariesManager->currentProgramType == 1)
		{
			newPolygon->dlines.cadMode = false;
			polygon->clear();
			modelPositions = binariesManager->readModel();
			polygon->addSet(modelPositions);
			//polygon->sweepTriangulation();
		}
		else if (binariesManager->currentProgramType == 0)
		{
			newPolygon->dlines.clear();
			newPolygon->dlines.cadAddSet(cursor);

			newPolygon->dlines.cadMode = true;
			modelPositions.clear();


		}
		counterI = 0;

	}
	//CTRL+S
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS &&
		key == GLFW_KEY_S && action == GLFW_PRESS) {

		binariesManager->writeModel(polygon->positions);



	}

	if (key == GLFW_KEY_O && action == GLFW_PRESS)
	{

		binariesManager->readConfig();
	}
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	AllPointers* allpointers = static_cast<AllPointers*>(glfwGetWindowUserPointer(window));

	NewPolygons* newPolygon = allpointers->newPolygon;
	Polygons* polygon = allpointers->polygon;
	BinariesManager* binariesManager = allpointers->binariesManager;

	

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		if (binariesManager->currentProgramType == binariesManager->CAD)
		{
			if (newPolygon->dlines.cadMode && newPolygon->positions.size() > 2
				&& newPolygon->positions.front() == newPolygon->positions.back())
			{
				newPolygon->dlines.cadMode = false;


			}
			else
			{
				if (newPolygon->dlines.cadMode == false)
				{
					newPolygon->dlines.cadMode = true;
					newPolygon->dlines.cadAddSet(cursor);
				}



				newPolygon->dlines.cadAddSet(cursor);

				
			}

			//cout << "cursorX: " << cursorX << ", cursorY: " << cursorY << endl;
			printv2(newPolygon->dlines.positions);
			printflat2(newPolygon->dlines.indices);


		}

	}
}