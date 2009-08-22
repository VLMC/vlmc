#include "WorkflowFileRendererDialog.h"

WorkflowFileRendererDialog::WorkflowFileRendererDialog()
{
    m_ui.setupUi( this );
}

void    WorkflowFileRendererDialog::setOutputFileName( const QString& outputFileName )
{
    m_ui.nameLabel->setText( outputFileName );
    setWindowTitle( "Rendering to " + outputFileName );
}

void    WorkflowFileRendererDialog::setProgressBarValue( int val )
{
    m_ui.progressBar->setValue( val );
}
