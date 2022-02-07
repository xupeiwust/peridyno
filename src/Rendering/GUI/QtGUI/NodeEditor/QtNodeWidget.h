#pragma once

#include <QtCore/QObject>
#include <QtCore/QJsonObject>
#include <QtWidgets/QLabel>

#include "nodes/QNodeDataModel"

#include "Node.h"
#include "QtNodeData.h"
#include "QtFieldData.h"

#include <iostream>


using dyno::Node;

namespace Qt
{
	/// The model dictates the number of inputs and outputs for the Node.
	/// In this example it has no logic.
	class QtNodeWidget : public QtNodeDataModel
	{
		Q_OBJECT

	public:
		QtNodeWidget(std::shared_ptr<Node> base = nullptr);

		virtual	~QtNodeWidget();

	public:

		QString caption() const override;

		QString name() const override;

		QString	portCaption(PortType portType, PortIndex portIndex) const override;

		QString	validationMessage() const override;


		unsigned int nPorts(PortType portType) const override;


		bool portCaptionVisible(PortType portType, PortIndex portIndex) const override;

		std::shared_ptr<QtNodeData> outData(PortIndex port) override;
		std::shared_ptr<QtNodeData> inData(PortIndex port);

		std::vector<FBase*>& getOutputFields() const;
		std::vector<FBase*>& getInputFields() const;

		void setInData(std::shared_ptr<QtNodeData> data, PortIndex portIndex) override;

		bool tryInData(PortIndex portIndex, std::shared_ptr<QtNodeData> nodeData) override;

		NodeDataType dataType(PortType portType, PortIndex portIndex) const override;


		QWidget* embeddedWidget() override { return nullptr; }

		NodeValidationState validationState() const override;

		QtNodeDataModel::ConnectionPolicy portInConnectionPolicy(PortIndex portIndex) const;

		std::shared_ptr<Node> getNode();

	protected:
		virtual void updateModule();

	protected:
		using ExportNodePtr = std::shared_ptr<QtNodeExportData>;
		using ImportNodePtr = std::vector<std::shared_ptr<QtNodeImportData>>;

		ImportNodePtr im_nodes;
		ExportNodePtr ex_node;

		using OutFieldPtr = std::vector<std::shared_ptr<QtFieldData>>;
		using InFieldPtr = std::vector<std::shared_ptr<QtFieldData>>;
		InFieldPtr input_fields;
		OutFieldPtr output_fields;

		std::shared_ptr<Node> m_node = nullptr;

		NodeValidationState modelValidationState = NodeValidationState::Valid;
		QString modelValidationError = QString("Missing or incorrect inputs");

	private:
	};


}
