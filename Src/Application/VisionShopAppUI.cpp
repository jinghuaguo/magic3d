#include "VisionShopAppUI.h"
#include "../Common/ResourceManager.h"
#include "../Common/LogSystem.h"
#include "../Common/ToolKit.h"
#include "../Common/AppManager.h"
#include "VisionShopApp.h"

namespace MagicApp
{
    VisionShopAppUI::VisionShopAppUI() :
        mpImageTexture(NULL)
    {
    }

    VisionShopAppUI::~VisionShopAppUI()
    {
        if (mpImageTexture != NULL)
        {
            MyGUI::RenderManager::getInstance().destroyTexture(mpImageTexture);
            mpImageTexture = NULL;
        }
    }

    void VisionShopAppUI::Setup()
    {
        InfoLog << "VisionShopAppUI::Setup" << std::endl;
        MagicCore::ResourceManager::LoadResource("../../Media/VisionShopApp", "FileSystem", "VisionShopApp");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("VisionShopApp.layout");
        mRoot.at(0)->findWidget("But_OpenImage")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &VisionShopAppUI::OpenImage);
        mRoot.at(0)->findWidget("But_SaveImage")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &VisionShopAppUI::SaveImage);
        mRoot.at(0)->findWidget("But_ImageResizing")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &VisionShopAppUI::ImageResizing);
        mRoot.at(0)->findWidget("But_ImageResizingDo")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &VisionShopAppUI::DoImageResizing);
        mRoot.at(0)->findWidget("But_FastImageResizingDo")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &VisionShopAppUI::FastImageResizing);
        mRoot.at(0)->findWidget("But_SaliencyDetection")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &VisionShopAppUI::SaliencyDetection);
        mRoot.at(0)->findWidget("But_Segment")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &VisionShopAppUI::SegmentImage);
        mRoot.at(0)->findWidget("But_BrushFront")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &VisionShopAppUI::BrushFront);
        mRoot.at(0)->findWidget("But_BrushBack")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &VisionShopAppUI::BrushBack);
        mRoot.at(0)->findWidget("But_SegmentDo")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &VisionShopAppUI::SegmentImageDo);
        mRoot.at(0)->findWidget("But_BackHome")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &VisionShopAppUI::BackHome);
        mRoot.at(0)->findWidget("But_Contact")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &VisionShopAppUI::Contact);
    }

    void VisionShopAppUI::Shutdown()
    {
        if (mpImageTexture != NULL)
        {
            MyGUI::RenderManager::getInstance().destroyTexture(mpImageTexture);
            mpImageTexture = NULL;
        }
        MyGUI::LayoutManager::getInstance().unloadLayout(mRoot);
        mRoot.clear();
        MagicCore::ResourceManager::UnloadResource("VisionShopApp");
    }

    void VisionShopAppUI::UpdateImageTexture(const cv::Mat& image)
    {
        int texW = image.cols;
        int texH = image.rows;
        if (mpImageTexture != NULL)
        {
            MyGUI::RenderManager::getInstance().destroyTexture(mpImageTexture);
            mpImageTexture = NULL;
        }
        mpImageTexture = MyGUI::RenderManager::getInstance().createTexture("ImageTexture");
        mpImageTexture->createManual(texW, texH, MyGUI::TextureUsage::Static | MyGUI::TextureUsage::Write, MyGUI::PixelFormat::R8G8B8A8);
        MyGUI::uint8* pDest = static_cast<MyGUI::uint8*>(mpImageTexture->lock(MyGUI::TextureUsage::Write));
        for (int yid = 0; yid < texH; yid++)
        {
            for (int xid = 0; xid < texW; xid++)
            {
                const unsigned char* pixel = image.ptr(yid, xid);
                *pDest++ = pixel[0];
                *pDest++ = pixel[1];
                *pDest++ = pixel[2];
                *pDest++ = 255;
            }
        }
		mpImageTexture->unlock();

        MyGUI::ImageBox* pIB = mRoot.at(0)->findWidget("Image_ImageView")->castType<MyGUI::ImageBox>();
        pIB->setSize(texW, texH);
        pIB->setImageTexture("ImageTexture");
    }

    void VisionShopAppUI::UpdateMarkedImageTexture(const cv::Mat& image, const cv::Mat& mark)
    {
        int texW = image.cols;
        int texH = image.rows;
        if (mpImageTexture != NULL)
        {
            MyGUI::RenderManager::getInstance().destroyTexture(mpImageTexture);
            mpImageTexture = NULL;
        }
        mpImageTexture = MyGUI::RenderManager::getInstance().createTexture("ImageTexture");
        mpImageTexture->createManual(texW, texH, MyGUI::TextureUsage::Static | MyGUI::TextureUsage::Write, MyGUI::PixelFormat::R8G8B8A8);
        MyGUI::uint8* pDest = static_cast<MyGUI::uint8*>(mpImageTexture->lock(MyGUI::TextureUsage::Write));
        for (int yid = 0; yid < texH; yid++)
        {
            for (int xid = 0; xid < texW; xid++)
            {
                const unsigned char* markPixel = mark.ptr(yid, xid);
                if (markPixel[0] > 0 || markPixel[1] > 0 || markPixel[2] > 0)
                {
                    *pDest++ = markPixel[0];
                    *pDest++ = markPixel[1];
                    *pDest++ = markPixel[2];
                    *pDest++ = 255;
                }
                else
                {
                    const unsigned char* pixel = image.ptr(yid, xid);
                    *pDest++ = pixel[0];
                    *pDest++ = pixel[1];
                    *pDest++ = pixel[2];
                    *pDest++ = 255;
                }
            }
        }
		mpImageTexture->unlock();

        MyGUI::ImageBox* pIB = mRoot.at(0)->findWidget("Image_ImageView")->castType<MyGUI::ImageBox>();
        pIB->setSize(texW, texH);
        pIB->setImageTexture("ImageTexture");
    }

    void VisionShopAppUI::OpenImage(MyGUI::Widget* pSender)
    {
        VisionShopApp* pVS = dynamic_cast<VisionShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("VisionShopApp"));
        if (pVS != NULL)
        {
            int w, h;
            if (pVS->OpenImage(w, h))
            {
                mRoot.at(0)->findWidget("But_SaveImage")->castType<MyGUI::Button>()->setEnabled(true);
                mRoot.at(0)->findWidget("But_ImageResizing")->castType<MyGUI::Button>()->setEnabled(true);
                mRoot.at(0)->findWidget("But_SaliencyDetection")->castType<MyGUI::Button>()->setEnabled(true);
                mRoot.at(0)->findWidget("But_Segment")->castType<MyGUI::Button>()->setEnabled(true);
                std::stringstream ss;
                std::string textString;
                ss << w;
                ss >> textString;
                mRoot.at(0)->findWidget("Edit_Width")->castType<MyGUI::EditBox>()->setOnlyText(textString);
                ss.clear();
                textString.clear();
                ss << h;
                ss >> textString;
                mRoot.at(0)->findWidget("Edit_Height")->castType<MyGUI::EditBox>()->setOnlyText(textString);
            }
        }
    }

    void VisionShopAppUI::SaveImage(MyGUI::Widget* pSender)
    {
        VisionShopApp* pVS = dynamic_cast<VisionShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("VisionShopApp"));
        if (pVS != NULL)
        {
            pVS->SaveImage();
        }
    }

    void VisionShopAppUI::ImageResizing(MyGUI::Widget* pSender)
    {
        bool isVisiable = mRoot.at(0)->findWidget("Edit_Width")->castType<MyGUI::EditBox>()->isVisible();
        mRoot.at(0)->findWidget("Edit_Width")->castType<MyGUI::EditBox>()->setVisible(!isVisiable);
        mRoot.at(0)->findWidget("Edit_Height")->castType<MyGUI::EditBox>()->setVisible(!isVisiable);
        mRoot.at(0)->findWidget("But_ImageResizingDo")->castType<MyGUI::Button>()->setVisible(!isVisiable);
        mRoot.at(0)->findWidget("But_FastImageResizingDo")->castType<MyGUI::Button>()->setVisible(!isVisiable);
    }

    void VisionShopAppUI::SaliencyDetection(MyGUI::Widget* pSender)
    {
        VisionShopApp* pVS = dynamic_cast<VisionShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("VisionShopApp"));
        if (pVS != NULL)
        {
            pVS->SaliencyDetection();
        }
    }

    void VisionShopAppUI::DoImageResizing(MyGUI::Widget* pSender)
    {
        VisionShopApp* pVS = dynamic_cast<VisionShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("VisionShopApp"));
        if (pVS != NULL)
        {
            std::string textString = mRoot.at(0)->findWidget("Edit_Width")->castType<MyGUI::EditBox>()->getOnlyText();
            int w = std::atoi(textString.c_str());
            textString = mRoot.at(0)->findWidget("Edit_Height")->castType<MyGUI::EditBox>()->getOnlyText();
            int h = std::atoi(textString.c_str());
            pVS->ImageResizing(w, h);
        }
    }

    void VisionShopAppUI::FastImageResizing(MyGUI::Widget* pSender)
    {
        VisionShopApp* pVS = dynamic_cast<VisionShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("VisionShopApp"));
        if (pVS != NULL)
        {
            std::string textString = mRoot.at(0)->findWidget("Edit_Width")->castType<MyGUI::EditBox>()->getOnlyText();
            int w = std::atoi(textString.c_str());
            textString = mRoot.at(0)->findWidget("Edit_Height")->castType<MyGUI::EditBox>()->getOnlyText();
            int h = std::atoi(textString.c_str());
            pVS->FastImageResizing(w, h);
        }
    }

    void VisionShopAppUI::SegmentImage(MyGUI::Widget* pSender)
    {
        bool isVisiable = mRoot.at(0)->findWidget("But_BrushFront")->castType<MyGUI::Button>()->isVisible();
        mRoot.at(0)->findWidget("But_BrushFront")->castType<MyGUI::Button>()->setVisible(!isVisiable);
        mRoot.at(0)->findWidget("But_BrushBack")->castType<MyGUI::Button>()->setVisible(!isVisiable);
        mRoot.at(0)->findWidget("But_SegmentDo")->castType<MyGUI::Button>()->setVisible(!isVisiable);
    }

    void VisionShopAppUI::BrushFront(MyGUI::Widget* pSender)
    {
        VisionShopApp* pVS = dynamic_cast<VisionShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("VisionShopApp"));
        if (pVS != NULL)
        {
            pVS->BrushFront();
        }
    }

    void VisionShopAppUI::BrushBack(MyGUI::Widget* pSender)
    {
        VisionShopApp* pVS = dynamic_cast<VisionShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("VisionShopApp"));
        if (pVS != NULL)
        {
            pVS->BrushBack();
        }
    }

    void VisionShopAppUI::SegmentImageDo(MyGUI::Widget* pSender)
    {
        VisionShopApp* pVS = dynamic_cast<VisionShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("VisionShopApp"));
        if (pVS != NULL)
        {
            pVS->SegmentImageDo();
        }
    }

    void VisionShopAppUI::BackHome(MyGUI::Widget* pSender)
    {
        MagicCore::AppManager::GetSingleton()->SwitchCurrentApp("Homepage");
    }

    void VisionShopAppUI::Contact(MyGUI::Widget* pSender)
    {
        MagicCore::ToolKit::OpenWebsite(std::string("http://magic3dcn.wix.com/magic3d#!form__map/c24vq"));
    }
}