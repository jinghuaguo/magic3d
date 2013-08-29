#include "StdAfx.h"
#include "PointSetViewerUI.h"
#include "../Common/ResourceManager.h"
#include "../Common/LogSystem.h"
#include "../Common/ToolKit.h"
#include "../DGP/Parser.h"
#include "../Common/AppManager.h"
#include "../Common/RenderSystem.h"
#include "PointSetViewer.h"

namespace MagicApp
{
    PointSetViewerUI::PointSetViewerUI()
    {
    }

    PointSetViewerUI::~PointSetViewerUI()
    {

    }

    void PointSetViewerUI::Setup()
    {
        MagicLog << "PointSetViewerUI::Setup" << std::endl;
        MagicCore::ResourceManager::GetSingleton()->LoadResource("../../Media/Pointviewer", "FileSystem", "Pointviewer");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("PointViewerLayout.layout");
        mRoot.at(0)->findWidget("But_OpenPointSet")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointSetViewerUI::OpenPointSet);
        mRoot.at(0)->findWidget("But_Back")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointSetViewerUI::BackToHomepage);
    }

    void PointSetViewerUI::Shutdown()
    {
        MyGUI::LayoutManager::getInstance().unloadLayout(mRoot);
        mRoot.clear();
        MagicCore::ResourceManager::GetSingleton()->UnloadResource("Pointviewer");
    }

    void PointSetViewerUI::OpenPointSet(MyGUI::Widget* pSender)
    {
        std::string fileName;
        MagicCore::ToolKit::GetSingleton()->FileOpenDlg(fileName);
        MagicDGP::Parser parser;
        MagicDGP::Point3DSet* pPointSet = parser.ParsePointSet(fileName);
        pPointSet->UnifyPosition(2.0);
        PointSetViewer* pPSViewer = dynamic_cast<PointSetViewer* >(MagicCore::AppManager::GetSingleton()->GetApp("PointSetViewer"));
        if (pPSViewer != NULL)
        {
            pPSViewer->SetPointSet(pPointSet);
            MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet(pPointSet, "test", "SimplePoint");
        }
    }

    void PointSetViewerUI::BackToHomepage(MyGUI::Widget* pSender)
    {
        MagicCore::AppManager::GetSingleton()->SwitchCurrentApp("Homepage");
    }
}