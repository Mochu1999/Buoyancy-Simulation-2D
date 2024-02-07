#pragma once



//change to command pattern
bool isCreating = true;
int counterI = 0;	//counterI ahora no hace nada, pero voy a dejar la logic para cuando testee agujeros
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	AllPointers* allpointers = static_cast<AllPointers*>(glfwGetWindowUserPointer(window));
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
			isCreating = true;
			polygon->clear();
			polygon->addSet({ cursorX,cursorY });
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
			/*cout << "polygon->dlines.positions: " << endl;
			for (int i = 0; i < polygon->dlines.positions.size(); i += 2) {
				cout << polygon->dlines.positions[i] << ", " << polygon->dlines.positions[i + 1] << "," << endl;
			}cout << endl;

			cout << "polygon->dlines.indices" << endl;
			for (unsigned int i = 0; i < polygon->dlines.indices.size(); i++) {
				cout << polygon->dlines.indices[i] << ", ";
			}cout << endl;*/

			polygon->sweepTriangulation();
			
		}
	}
	//CTRL+N
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS &&
		key == GLFW_KEY_N && action == GLFW_PRESS) {

		binariesManager->writeConfig();

		if (binariesManager->currentProgramType == 1)
		{
			isCreating = false;
			polygon->clear();
			polygon->addSet(binariesManager->readModel());
			//polygon->sweepTriangulation();
		}
		else if (binariesManager->currentProgramType == 0)
		{
			isCreating = true;
			polygon->clear();
			polygon->addSet({ cursorX,cursorY });
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
	Polygons* polygon = allpointers->polygon;
	BinariesManager* binariesManager = allpointers->binariesManager;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		if (binariesManager->currentProgramType == binariesManager->CAD)
		{
			if (polygon->positions.size() > 4 && polygon->positions[0] == polygon->positions[polygon->positions.size() - 2]
				&& polygon->positions.back() == polygon->positions[1])
			{
				isCreating = false;
			}
			else
			{
				polygon->dlines.addSet({ cursorX, cursorY, });

			}

			//cout << "cursorX: " << cursorX << ", cursorY: " << cursorY << endl;




		}

	}
}