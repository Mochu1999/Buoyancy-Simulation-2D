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
	for (p item : newPositions)
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
	Polygons* polygon = allpointers->polygon;
	BinariesManager* binariesManager = allpointers->binariesManager;


	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_P:
			isRunning = !isRunning;
			break;

		case GLFW_KEY_ESCAPE:
			if (binariesManager->currentProgramType == 1) {
				polygon->clear();
				polygon->addSet(model);
			}
			else if (binariesManager->currentProgramType == 0) {
				if (polygon->indices.size()) {
					polygon->clear();
					polygon->addSet(currentModel);
				}
				else {
					polygon->clear();
				}
			}
			counterI = 0;
			break;

		case GLFW_KEY_C:
			if (counterI < polygon->sPoints.size()) {
				polygon->sweepTriangulation();
				counterI++;
			}
			break;

		case GLFW_KEY_Q:
			switchOnePosition(model);
			break;

		case GLFW_KEY_SPACE:
			for (size_t i = 0; i < polygon->sPoints.size(); ++i) {
				polygon->sweepTriangulation();
			}
			printv2(polygon->positions);
			printflat3(polygon->indices);
			isRunning = true;
			break;

		case GLFW_KEY_O:
			binariesManager->readConfig();
			break;

		}

		///CTRL
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			switch (key)
			{
			case GLFW_KEY_N:

				binariesManager->writeConfig();

				if (binariesManager->currentProgramType == 1)
				{
					polygon->lines.cadMode = false;
					modelPositions = binariesManager->readModel();


					polygon->clear();
					model = convertPositions(modelPositions);
					polygon->addSet(model);

					isRunning = true;
				}
				else if (binariesManager->currentProgramType == 0)
				{
					polygon->clear();
					polygon->lines.cadAddSet(cursor);

					polygon->lines.cadMode = true;
					modelPositions.clear();


				}
				counterI = 0;

				break;

			case GLFW_KEY_S:
				auto oldFormatPositions = inverseTransforming(polygon->positions);
				printflat2(oldFormatPositions);
				binariesManager->writeModel(oldFormatPositions);
				break;

			}
		}
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
			if (polygon->lines.cadMode && polygon->positions.size() > 2
				&& polygon->positions.front() == polygon->positions.back())
			{

				polygon->lines.cadMode = false;

				currentModel = polygon->positions;

				polygon->clear();

				polygon->addSet(currentModel);
			}
			else
			{
				if (polygon->lines.cadMode == false)
				{
					polygon->lines.cadMode = true;
					polygon->lines.cadAddSet(cursor);
				}



				polygon->lines.cadAddSet(cursor);


			}

			//cout << "cursorX: " << cursorX << ", cursorY: " << cursorY << endl;
			printv2(polygon->lines.positions);
			printflat2(polygon->lines.indices);


		}

	}
}