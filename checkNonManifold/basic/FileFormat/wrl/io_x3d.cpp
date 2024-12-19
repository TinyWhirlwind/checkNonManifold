#include "io_x3d.h"

#include "import_x3d.h"
#include "export_x3d.h"

using namespace std;

bool IoX3DPlugin::open(const std::string &formatName, const std::string &fileName, sn3DMeshModel & m, int& mask)
{
    // initializing mask
    mask = 0;

    string filename = QFile::encodeName(fileName).constData ();
    bool normalsUpdated = false;
    
    if(formatName.toUpper() == tr("WRL"))
	{
        int result;
        if (formatName.toUpper() == tr("X3D"))
            result = vcg::tri::io::ImporterX3D<CMeshO>::LoadMask(filename.c_str(), info);
        else
            result = vcg::tri::io::ImporterX3D<CMeshO>::LoadMaskVrml(filename.c_str(), info);
        if ( result != vcg::tri::io::ImporterX3D<CMeshO>::E_NOERROR)
        {
            QMessageBox::critical(parent, tr("X3D Opening Error"), errorMsgFormat.arg(fileName, info->filenameStack[info->filenameStack.size()-1], vcg::tri::io::ImporterX3D<CMeshO>::ErrorMsg(result)));
            delete info;
            return false;
        }
        if (info->mask & vcg::tri::io::Mask::IOM_VERTTEXCOORD)
        {
            info->mask |= vcg::tri::io::Mask::IOM_WEDGTEXCOORD;
            info->mask &=(~vcg::tri::io::Mask::IOM_VERTTEXCOORD);
        }
        if (info->mask & vcg::tri::io::Mask::IOM_WEDGCOLOR)
            info->mask &=(~vcg::tri::io::Mask::IOM_WEDGCOLOR);
        if (info->mask & vcg::tri::io::Mask::IOM_WEDGNORMAL)
            info->mask &=(~vcg::tri::io::Mask::IOM_WEDGNORMAL);
        m.Enable(info->mask);

        errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nFile: %2\nLine number: %3\nError details: %4";
        result = vcg::tri::io::ImporterX3D<CMeshO>::Open(m.cm, filename.c_str(), info, cb);
        if (result != vcg::tri::io::ImporterX3D<CMeshO>::E_NOERROR)
        {
            QString fileError = info->filenameStack[info->filenameStack.size()-1];
            QString lineError;
            lineError.setNum(info->lineNumberError);
            QMessageBox::critical(parent, tr("X3D Opening Error"), errorMsgFormat.arg(fileName, fileError, lineError, vcg::tri::io::ImporterX3D<CMeshO>::ErrorMsg(result)));
            delete info;
            return false;
        }
        if (m.cm.vert.size() == 0)
        {
            errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nError details: File without a geometry";
            QMessageBox::critical(parent, tr("X3D Opening Error"), errorMsgFormat.arg(fileName));
            delete info;
            return false;
        }
        if(info->mask & vcg::tri::io::Mask::IOM_WEDGNORMAL)
            normalsUpdated = true;
        mask = info->mask;

        QString missingTextureFilesMsg = "The following texture files were not found:\n";
        bool someTextureNotFound = false;
        for(unsigned int tx = 0; tx < info->textureFile.size(); ++tx)
        {
            FILE* pFile = fopen (info->textureFile[tx].toStdString().c_str(), "r");
            if (pFile == NULL)
            {
                missingTextureFilesMsg.append("\n");
                missingTextureFilesMsg.append(info->textureFile[tx].toStdString().c_str());
                someTextureNotFound = true;
            }
            else
            {
                m.cm.textures.push_back(info->textureFile[tx].toStdString());
                fclose (pFile);
            }
        }

    }

    return true;
}
bool IoX3DPlugin::save(const std::string &formatName, const std::string &fileName, sn3DMeshModel & m, const int mask)
{
    /*string filename = QFile::encodeName(fileName).constData ();
    if(formatName.toUpper() == tr("X3D"))
    {
        int result = vcg::tri::io::ExporterX3D<CMeshO>::Save(m.cm, filename.c_str(), mask, cb);
        if(result!=0)
        {
           
            return false;
        }
        if (cb !=NULL) (*cb)(99, "Saving X3D File...");
        return true;
    }
    assert(0);*/
    return false;
}