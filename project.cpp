#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkSmartPointer.h>
#include <vtkVertexGlyphFilter.h>

#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkCellArray.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>

#include <vtkNamedColors.h>
int main(int argc, char* argv[])
{
	/*
	This is the string for the filename we would like to read.
	Is is important that the filename is adapted to the location of the source file on your own system.
	*/
	std::string filename = "/home/lilit/Documents/Visualization/git_rep/sagittarius_a/Full.cosmo.446.vtp";

	/*
	An object of class vtkXMLPolyDataReader is used to read .vtp files, as it is the case with our source file.
	.vtp files can contain all kinds of data, such as points, vertices, meshes and scalar arrays which save data corresponding to points.
	In our case, the dataset only contains points and scalar values as arrays mapped to the respective points.
	Even though that's the case, a .vtp file is still a good file to contain such data, although visualization of that type of data is not as easy as it might seem.
	*/
	vtkSmartPointer<vtkXMLPolyDataReader> reader =
		vtkSmartPointer<vtkXMLPolyDataReader>::New();
	reader->SetFileName(filename.c_str());
	reader->Update();

	/*
	The vtkPolyData object is the main objects containing data.
	We can set the contents of the polydata object, here called vtpData, to the things we're previously read with the line reader->GetOutput;
	vtpData now contains 
	*the points
	*all the scalar values associated to the points
	as read from the source file.
	It would also contain vertices and other topological data, but in our case, this data is not in the source file so there is no data of such type.
	*/
	vtkSmartPointer<vtkPolyData> vtpData = vtkSmartPointer<vtkPolyData>::New();
	vtpData = reader->GetOutput();

	/*
	The vtpData object contains various data Arrays; attached to every point are values with various names; the names we previously saw in ParaView
	I'm calling these Arrays because the name of their class is vtkDataArray.
	In this line, we create a new pointer on the array of density data (called rho).
	Since it has a name (which is set in the source data), we can access it by first using the GetPointData function in order to get a pointer to a vtkPointData object containing the points, and then using the GetArray function to get a pointer to the vtkDataArray object which contains the values of rho.
	*/
	vtkSmartPointer<vtkDataArray> rhoArray = vtpData->GetPointData()->GetArray("rho");


/*
This is some code which has been produced while debugging and writing this code; it has no complete relevance to the task at hand, hence I will document it later. We will keep it here for further use so far.
*/
#pragma region Probably useful
	/*
	// Setup colors
	vtkSmartPointer<vtkNamedColors> namedColors = vtkSmartPointer<vtkNamedColors>::New();
		// extract raw points from the poly-data object
		vtkSmartPointer < vtkPoints > vtpPoints = vtkSmartPointer<vtkPoints>::New();
		vtpPoints = vtpData->GetPoints();

		// assign the points to a new polydata object
		vtkSmartPointer<vtkPolyData> rawPoints = vtkSmartPointer<vtkPolyData>::New();
		rawPoints->SetPoints(vtpPoints);
	*/
#pragma endregion

	/*
	a vtkVertexGlyphFilter, according to the documentation, does the following:
	This filter throws away all of the cells in the input and replaces them with a vertex on each point. The intended use of this filter is roughly equivalent to the vtkGlyph3D filter, except this filter is specifically for data that has many vertices, making the rendered result faster and less cluttered than the glyph filter. This filter may take a graph or point set as input.
	Our initial data set has points but no vertices saved.
	In order to be able to visualize it, we need to create vertices.
	In our case we simply want to create vertices wherever we have a data point, hence this filter is perfect for us.
	We end up with an object which now has points at the vertices, which have previously not been here.
	*/
	vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter =
		vtkSmartPointer<vtkVertexGlyphFilter>::New();
	vertexFilter->SetInputData(vtpData);
	vertexFilter->Update();

	/*
	We now create a second polydata object, and we want to store the output of the filter in this object.
	In order to do that we use the vtkPolyData::ShallowCopy function, which does merely set a pointer and does not copy anything, and is hence more efficient than fully copying it.
	After these lines, polydata contains the information filtered by the VertexGlyphFilter.
	*/
	vtkSmartPointer<vtkPolyData> polydata =
		vtkSmartPointer<vtkPolyData>::New();
	polydata->ShallowCopy(vertexFilter->GetOutput());


/*
	This is again some silly code which is not directly relevant for the task at hand, hence I will not document is for now.
	Ask me (Aaron) if you want to know more about it.
*/
#pragma region Probably useful
	/*
	// Setup the colors array
	vtkSmartPointer<vtkUnsignedCharArray> colorArray = vtkSmartPointer<vtkUnsignedCharArray>::New();

	colorArray->SetNumberOfComponents(3);
	colorArray->SetName("Colors");

	for (int i = 0; i < polydata->GetNumberOfPoints(); i += 3) {
		//colorArray->InsertNextTupleValue(namedColors->GetColor3ub("Tomato").GetData());
		colorArray->InsertNextTypedTuple(namedColors->GetColor3ub("Tomato").GetData());
		colorArray->InsertNextTypedTuple(namedColors->GetColor3ub("Mint").GetData());
		colorArray->InsertNextTypedTuple(namedColors->GetColor3ub("Peacock").GetData());
	}
	*/
	//polydata->GetPointData()->SetScalars(colorArray);
#pragma endregion


	/*
	The polydata object of type vtkPolyData now contains the vertices generated by the filter and the points.
	We now attach a scalar value to all the points, and the scalar value we use is the density.
	The argument we pass is a pointer to the vtkDataArray of the values we would like to attach.
	In our case, we attach the density values from the dataset.
	*/
	polydata->GetPointData()->SetScalars(rhoArray);

	/*
	We now create the mapper, as usual, and use polydata as the information source to map.
	Ultimately, the vertices are mapped; which were generated from the filter.
	*/
	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(polydata);

	/*
	The actor is the second step in the rendering pipeline.
	We can change the size of the vertex points with the line actor->GetProperty()->SetPointSize(2);
	Try playing around with it.
	The mapper also appears to automatically map the attached scalar value to a color; this appears to be its default behavior.
	The next step is to understand and tweak this behavior.
	*/
	vtkSmartPointer<vtkActor> actor =
		vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->SetPointSize(2);


	/*
	The rest of the rendering pipeline is unspectacular and should be kinda familiar from the coding exercises.
	They don't do anything particularly interesting here; just rendering.
	*/
	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	renderer->AddActor(actor);
	renderer->SetBackground(0.7, 0.7, 0.7);
	renderWindow->Render();
	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}
