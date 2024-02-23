#pragma once

void switchOnePosition(vector<p>& positions) {

	printv2(positions);
	positions.erase(positions.begin());
	positions.insert(positions.end(), { positions.front() });
	printv2(positions);
	cout << endl;
}

vector<float> inverseTransforming(vector<p> newPositions) {
	vector<float> output;
	for (p item :newPositions )
	{
		output.insert(output.end(), { item.x,item.y });
	}
	return output;
}

//change to command pattern

vector<p>currentModel;

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

	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if(continueRunning)
			continueRunning = false;
		else
			continueRunning = true;
	}

	//SCAPE
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		//glfwSetWindowShouldClose(window, GLFW_TRUE);


		if (binariesManager->currentProgramType == 1)
		{

			newPolygon->clear();
			newPolygon->addSet(model);

		}
		else if (binariesManager->currentProgramType == 0)
		{
			
			if (newPolygon->indices.size())
			{
				newPolygon->clear();
				newPolygon->addSet(currentModel);

			}
			else
			{
				newPolygon->clear();
			}
		}
		counterI = 0;
	}
	//C
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{


		if (counterI < newPolygon->Points.size())
		{
			//polygon->sweepTriangulation(/*counterI*/);

			newPolygon->sweepTriangulation(/*counterI*/);
			counterI++;


		}

	}
	//Q
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		switchOnePosition(model);
	}

	//SPACE
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{

		for (size_t i = 0; i < newPolygon->Points.size(); ++i)
		{
			polygon->sweepTriangulation();
			

		}
		newPolygon->sweepTriangulation(/*i*/);

		printv2(newPolygon->positions);
		printflat3(newPolygon->indices);
		continueRunning = true;
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
			
			newPolygon->clear();
			model = convertPositions(modelPositions);
			newPolygon->addSet(model);
			
			continueRunning = true;
		}
		else if (binariesManager->currentProgramType == 0)
		{
			newPolygon->clear();
			newPolygon->dlines.cadAddSet(cursor);

			newPolygon->dlines.cadMode = true;
			modelPositions.clear();


		}
		counterI = 0;

	}
	//CTRL+S
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS &&
		key == GLFW_KEY_S && action == GLFW_PRESS) {

		auto oldFormatPositions = inverseTransforming(newPolygon->positions);

		printflat2(oldFormatPositions);
		binariesManager->writeModel(oldFormatPositions);



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

				currentModel = newPolygon->positions;

				newPolygon->clear();

				newPolygon->addSet(currentModel);
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