#include "EngineOptions.h"

#include <DirectXMath.h>
#include "tinyxml.h"
#include <memory>

EngineOptions::EngineOptions() {

	// set all the options to decent default valu
	m_Renderer				= Renderer::Renderer_D3D11;
	m_runFullSpeed			= false;
	m_fullScreen			= false;
	m_soundEffectsVolume	= 1.0f;
	m_musicVolume			= 1.0f;
	m_screenDepth			= 1000.0f;
	m_screenNear			= 0.1f;
	m_fov					= DirectX::XM_PIDIV2;
	m_aspectRatio			= 1.0f;
}

EngineOptions::~EngineOptions() {}

void EngineOptions::Init(const char* xmlFileName, LPWSTR lpCmdLine) {
	// read the XML file
	// if needed, override the XML file with options passed in on the command line.

	std::shared_ptr<TiXmlDocument> m_pDoc(new TiXmlDocument(xmlFileName));

	
	if (m_pDoc && m_pDoc->LoadFile()) {
		TiXmlElement* pRoot = m_pDoc->RootElement();
		if (!pRoot) {
			return;
		}

		// Loop through each child element and load the component
		TiXmlElement* pNode = NULL;
		pNode = pRoot->FirstChildElement("Graphics");
		if (pNode) {
			std::string attribute;

			attribute = pNode->Attribute("renderer");
			if (attribute == "Direct3D 11") {
				m_Renderer = Renderer::Renderer_D3D11;
			}

			if (pNode->Attribute("width")) {
				m_screenWidth = atoi(pNode->Attribute("width"));
			}

			if (pNode->Attribute("height")) {
				m_screenHeight = atoi(pNode->Attribute("height"));
			}

			if (m_screenHeight != 0) {
				m_aspectRatio = static_cast<float>(m_screenWidth) / static_cast<float>(m_screenHeight);
			}

			if (pNode->Attribute("runfullspeed")) {
				attribute = pNode->Attribute("runfullspeed");
				m_runFullSpeed = (attribute == "yes") ? true : false;
			}

			if (pNode->Attribute("fullscreen")) {
				attribute = pNode->Attribute("fullscreen");
				m_fullScreen = (attribute == "yes") ? true : false;
			}

			if (pNode->Attribute("screenDepth")) {
				m_screenDepth = atof(pNode->Attribute("screenDepth"));
			}

			if (pNode->Attribute("screenNear")) {
				m_screenNear = atof(pNode->Attribute("screenNear"));
			}
		}

		pNode = pRoot->FirstChildElement("Sound");
		if (pNode) {
			m_musicVolume = atoi(pNode->Attribute("musicVolume")) / 100.0f;
			m_soundEffectsVolume = atoi(pNode->Attribute("sfxVolume")) / 100.0f;
		}
	}
}