#include <vtkStructuredPointsReader.h>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkWidgetEvent.h>
#include <vtkRenderer.h>
#include <vtkPolyData.h>
#include <vtkPlane.h>
#include <vtkCutter.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkContourFilter.h>
#include <vtkSliderWidget.h>
#include <vtkCommand.h>
#include <vtkProperty.h>
#include <vtkCleanPolyData.h>
#include <vtkSliderRepresentation3D.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>


class vtkSliderCallback : public vtkCommand
{
public:
	static vtkSliderCallback *New()
	{
		return new vtkSliderCallback;
	}
	virtual void Execute(vtkObject *caller, unsigned long, void*)
	{
		vtkSliderWidget *sliderWidget =
			reinterpret_cast<vtkSliderWidget*>(caller);
		this->visual->SetValue(0, static_cast<vtkSliderRepresentation *>(sliderWidget->GetRepresentation())->GetValue());
	}
	vtkSliderCallback() :visual(0) {}
	vtkContourFilter *visual;
};
int main(int, char *argv[])
{
	vtkSmartPointer<vtkStructuredPointsReader> stucturedPointReader = vtkSmartPointer<vtkStructuredPointsReader>::New();
	stucturedPointReader->SetFileName("ironProt.vtk");
	stucturedPointReader->Update();

	vtkSmartPointer<vtkContourFilter> counterFiller = vtkSmartPointer<vtkContourFilter>::New();
	counterFiller->SetInputConnection(stucturedPointReader->GetOutputPort());
	counterFiller->SetValue(0, 128);

	vtkSmartPointer<vtkCleanPolyData> cleanPolydata = vtkSmartPointer<vtkCleanPolyData>::New();
	cleanPolydata->SetInputConnection(counterFiller->GetOutputPort());

	vtkSmartPointer<vtkPolyDataNormals> polyDatanormals = vtkSmartPointer<vtkPolyDataNormals>::New();
	polyDatanormals->SetInputConnection(cleanPolydata->GetOutputPort());
	polyDatanormals->SetFeatureAngle(45);

	vtkSmartPointer<vtkPolyDataMapper> polyDatamapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	polyDatamapper->SetInputConnection(polyDatanormals->GetOutputPort());

	vtkSmartPointer<vtkActor> actor1 = vtkSmartPointer<vtkActor>::New();
	actor1->SetMapper(polyDatamapper);
	actor1->GetProperty()->SetColor(0, 0, 1);

	vtkSmartPointer<vtkPlane> planee = vtkSmartPointer<vtkPlane>::New();
	planee->SetNormal(1, 0, 0);
	planee->SetOrigin(15, 0, 0);


	vtkSmartPointer<vtkCutter> cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetCutFunction(planee);
	cutter->SetInputConnection(stucturedPointReader->GetOutputPort());
	cutter->Update();


	vtkSmartPointer<vtkPolyDataMapper> pdataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	pdataMapper->SetInputConnection(cutter->GetOutputPort());


	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkRenderWindowInteractor> rendererWindowinteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	rendererWindowinteractor->SetRenderWindow(renderWindow);


	double leftViewport[4] = { 0.0, 0.0, 0.3, 0.7 };
	double rightViewport[4] = { 0.3, 0.0, 0.7, 0.7 };



	vtkSmartPointer<vtkRenderer> rendererr =
		vtkSmartPointer<vtkRenderer>::New();
	renderWindow->AddRenderer(rendererr);
	rendererr->SetBackground(.2, .5, .5);
	rendererr->SetViewport(rightViewport);

	vtkSmartPointer<vtkRenderer> renderer2 = vtkSmartPointer<vtkRenderer>::New();
	renderWindow->AddRenderer(renderer2);
	renderer2->SetBackground(.4, .3, .7);
	renderer2->SetViewport(leftViewport);


	vtkSmartPointer<vtkActor> actor2 = vtkSmartPointer<vtkActor>::New();
	actor2->GetProperty()->SetColor(0, 1.5, 1);
	actor2->SetMapper(pdataMapper);
	actor2->GetProperty()->SetLineWidth(2);



	rendererr->AddActor(actor1);
	renderer2->AddActor(actor2);



	vtkSmartPointer<vtkSliderRepresentation3D> sloderRepresentation3d = vtkSmartPointer<vtkSliderRepresentation3D>::New();
	sloderRepresentation3d->SetMinimumValue(0.0);
	sloderRepresentation3d->SetMaximumValue(128.0);
	sloderRepresentation3d->SetValue(128);
	sloderRepresentation3d->SetTitleText(" data set");
	sloderRepresentation3d->SetPoint1InWorldCoordinates(-25, -30, 0);
	sloderRepresentation3d->SetPoint2InWorldCoordinates(25, -30, 0);
	sloderRepresentation3d->SetSliderLength(0.080);
	sloderRepresentation3d->SetSliderWidth(0.1);
	sloderRepresentation3d->SetEndCapLength(0.1);

	vtkSmartPointer<vtkSliderWidget> sliderwidget = vtkSmartPointer<vtkSliderWidget>::New();
	sliderwidget->SetInteractor(rendererWindowinteractor);
	sliderwidget->SetRepresentation(sloderRepresentation3d);
	sliderwidget->SetAnimationModeToAnimate();
	sliderwidget->EnabledOn();

	vtkSmartPointer<vtkSliderCallback> sliderCallback = vtkSmartPointer<vtkSliderCallback>::New();
	sliderCallback->visual = counterFiller;

	sliderwidget->AddObserver(vtkCommand::InteractionEvent, sliderCallback);

	rendererWindowinteractor->Initialize();
	renderWindow->Render();
	rendererWindowinteractor->Start();
	return EXIT_SUCCESS;
}