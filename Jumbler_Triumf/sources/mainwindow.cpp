#include "mainwindow.h"

MainWindow::MainWindow()
{
    readSettings();

    window = new QWidget;
    setCentralWidget(window);
    window->resize(640,360);
    QGridLayout *mainLayout = new QGridLayout(window);

    fileName = new QLabel("No file to be analyzed set!");
    QLabel* fileName1=new QLabel("Data Files:");
    if(!fileAddress.at(0).isEmpty())
    {
        if(fileAddress.size()>5)
        {
            shortAddress.clear();
            for(int i=0;i<5;i++)
                shortAddress.append(fileAddress[i]);
            fileName->setText(QObject::tr("%1\n ... \n And %2 more files").arg(shortAddress.join("\n")).arg(fileAddress.size()-5));
        }
        else fileName->setText(fileAddress.join("\n"));
    }

    fileNameButton = new QPushButton(QObject::tr("Choose"));
    mainLayout->addWidget(fileName1,0,0,1,1);
    mainLayout->addWidget(fileName, 0,1,1,2);
    mainLayout->addWidget(fileNameButton, 0,3,1,1);
    connect(fileNameButton,SIGNAL(clicked()),this,SLOT(fileButtonClicked()));

    QFrame* myFrame1 = new QFrame();
    myFrame1->setFrameShape(QFrame::HLine);
    mainLayout->addWidget(myFrame1, 1, 0, 1, 4);

    outName = new QLabel("No output folder selected!");
    QLabel* outName1=new QLabel("Output folder:");
    if(!outAddress.isEmpty())
        outName->setText(outAddress);
    outNameButton = new QPushButton(QObject::tr("Choose"));
    mainLayout->addWidget(outName1,2,0,1,1);
    mainLayout->addWidget(outName, 2,1,1,2);
    mainLayout->addWidget(outNameButton, 2,3,1,1);
    connect(outNameButton,SIGNAL(clicked()),this,SLOT(outButtonClicked()));
    
    QFrame* myFrame5 = new QFrame();
    myFrame5->setFrameShape(QFrame::HLine);
    mainLayout->addWidget(myFrame5, 3, 0, 1, 4);

    areCloverLabel = new QLabel("Are these clovers?");
    areCloverCheckBox = new QCheckBox();
    if(areClover)
        areCloverCheckBox->setChecked(1);
    else
        areCloverCheckBox->setChecked(0);
    mainLayout->addWidget(areCloverLabel,4,0,1,1);
    mainLayout->addWidget(areCloverCheckBox, 4,3,1,1);
    connect(areCloverCheckBox,SIGNAL(clicked()),this,SLOT(cloverClicked()));

    QFrame* myFrame6 = new QFrame();
    myFrame6->setFrameShape(QFrame::HLine);
    mainLayout->addWidget(myFrame6, 5, 0, 1, 4);
    
    runButton = new QPushButton(QObject::tr("Run!"));
    mainLayout->addWidget(runButton, 6,0,1,4);
    connect(runButton,SIGNAL(clicked()),this,SLOT(runButtonClicked()));

    progressLabel = new QLabel("Ready to go");
    progressLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(progressLabel, 7,0,1,5);
    progressLabel->setAlignment(Qt::AlignCenter);
}

void MainWindow::closeEvent(QCloseEvent *event)
{

    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Jumbler",
                                                                   tr("Do you want to save?\n"),
                                                                   QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                   QMessageBox::Yes);
       if (resBtn == QMessageBox::Yes) {
           writeSettings();
           event->accept();
       }

       else if (resBtn == QMessageBox::No)
       {
           event->accept();
       }
       else
       {
           event->ignore();
       }
}

void MainWindow::readSettings()
{        
    QSettings settings("IFIN-HH", "Jumbler");
        settings.beginGroup("MainWindow");
        fileAddress=settings.value("File Name").toString().split("\n");
        outAddress=settings.value("Output Folder").toString();
        areClover=settings.value("Are Clover").toBool();
}


void MainWindow::writeSettings()
{
    QSettings settings("IFIN-HH", "Jumbler");

    settings.beginGroup("MainWindow");
    settings.setValue("File Name", fileAddress.join("\n"));
    settings.setValue("Output Folder", outAddress);
    settings.setValue("Are Clover", areClover);
    settings.endGroup();
}

void MainWindow::fileButtonClicked()
{
    fileAddress=QFileDialog::getOpenFileNames(0,QObject::tr("Choose data file name"), "/home/Desktop/Jumbler",QObject::tr("Text (*)"));
    if(fileAddress.size()>5)
    {
        shortAddress.clear();
        for(int i=0;i<5;i++)
            shortAddress.append(fileAddress[i]);
        fileName->setText(QObject::tr("%1\n And %2 more files").arg(shortAddress.join("\n")).arg(fileAddress.size()-5));
    }
    else fileName->setText(fileAddress.join("\n"));
}

void MainWindow::outButtonClicked()
{
    outAddress=QFileDialog::getExistingDirectory(0,QObject::tr("Choose output folder"),"/home",QFileDialog::ShowDirsOnly);
    outName->setText(outAddress);
}

void MainWindow::runButtonClicked()
{
    readData();

    progressLabel->setText(QObject::tr("Success!"));
}

void MainWindow::cloverClicked()
{
    areClover=areCloverCheckBox->isChecked();
}

void MainWindow::readData()
{
    int index,length;
    QVector <u_int16_t> det;
    QVector <u_int16_t> energy;
    QVector <Signal> detSignal;
    Signal standIn;

    srand(time(NULL));
    QDataStream input;
    QFile inputFile;
    uint16_t *fhead;
    QString comments;

    for(int k=0;k<fileAddress.size();k++)
    {
        std::cout<<fileAddress[k].toStdString()<<std::endl;
        inputFile.setFileName(fileAddress[k]);
        inputFile.open(QIODevice::ReadOnly);

        fileNumber = fileAddress[k].right(4).toInt();

        input.setDevice(&inputFile);
        input.setByteOrder(QDataStream::LittleEndian);

        outputFile.close();
        outputFile.setFileName(QObject::tr("%1/Mixed%2").arg(outAddress).arg(fileNumber,4,10,QChar('0')));
        outputFile.open(QIODevice::WriteOnly);

        output.setDevice(&outputFile);
        output.setByteOrder(QDataStream::LittleEndian);

        fhead=( u_int16_t *) calloc(16, sizeof(u_int16_t *));
        comments.resize(16352);

        //Each block, including the file header, must be 16k bytes long. Comments is the file header without the block header
        for(int k=0;k<16352;k++)
            comments[k]=0;

        //Create the block header of the file header
        fhead[0]= 16;
        fhead[1]= 0;
        fhead[2]= fileNumber;
        fhead[3]= 18248;
        fhead[4]= 16;

        //Write the block header of the file header
        for(int k=0;k<16;k++)
            output<<fhead[k];

        output.writeRawData( comments.toLatin1(), 16352);

        for(int k=0;k<8176;k++)
           cache[k]=0;
        written=0;

        if(inputFile.isOpen())
        {
            QVector<u_int16_t> read;
            read.resize(8192);

            //reading header, which does not contain useful information
            for(int i=0;i<8192;i++)
                input>>read[i];

            while(!input.atEnd())
            {
                //read a block of data
                read.clear();
                read.resize(8192);
                for(int i=0;i<8192;i++)
                    input>>read[i];

                //start index at 16, so we can skip the block header
                index=16;
                while(index<8192)
                {
                    if(read[index]!=0)
                    {
                        //get event size from event header
                        length=read[index]-61440;
                    }
                    else break;

                    if(read[index+1]>1)
                    {
                        det.clear();
                        energy.clear();

                        for(int i=0;i<read[index+1];i++)
                        {
                            det.append(read[index+1+1+i*2]);
                            energy.append(read[index+1+2+i*2]);
                        }

                        for(int i=0;i<det.size();i++)
                        {
                            standIn.id=det[i];
                            standIn.energy=energy[i];
                            standIn.reads=(det.size()-1)*20;
                            detSignal.append(standIn);
                        }


                    }

                    index=index+length+1;
                }

                if(detSignal.size()>100000)
                {
                    mixEvents(detSignal);
                    detSignal.clear();
                }

            }
            inputFile.close();
            mixEvents(detSignal);
            detSignal.clear();
            writeCache();
            outputFile.close();
        }
    }
}

void MainWindow::mixEvents(QVector <Signal> events)
{
    int rand1;
    int previousEventSize=0, stuck=0;

    while(events.size())
    {
            rand1=((double) rand() / (RAND_MAX))*(events.size()-1);
            if(events[0].id/4!=events[rand1].id/4 && areClover)
                {
                    if(written+7>8176)
                    {
                        writeCache();
                        for(int k=0;k<8176;k++)
                           cache[k]=0;
                        written=0;
                    }
                    cache[written++] = 0xF005;
                    cache[written++] = 2;
                    cache[written++] =events[0].id;
                    cache[written++] =events[0].energy;
                    cache[written++] =events[rand1].id;
                    cache[written++] =events[rand1].energy;

                    events[0].reads--;
                    if(events[0].reads==0)
                        events.remove(0);
                    events[rand1].reads--;
                    if(events[rand1].reads==0)
                    {
                        events.remove(rand1);
                    }
                }
            else if(events[0].id!=events[rand1].id && !areClover)
            {
                if(written+7>8176)
                {
                    writeCache();
                    for(int k=0;k<8176;k++)
                       cache[k]=0;
                    written=0;
                }
                cache[written++] = 0xF005;
                cache[written++] = 2;
                cache[written++] =events[0].id;
                cache[written++] =events[0].energy;
                cache[written++] =events[rand1].id;
                cache[written++] =events[rand1].energy;

                events[0].reads--;
                if(events[0].reads==0)
                    events.remove(0);
                events[rand1].reads--;
                if(events[rand1].reads==0)
                {
                    events.remove(rand1);
                }
            }

            if(previousEventSize==events.size())
            {
                stuck++;
            }
            else
            {
                previousEventSize=events.size();
                stuck=0;
            }

            if(stuck>10000)
            {
                break;
            }
    }
}

int MainWindow::writeCache(){
    //Check if the writing file is open
    if(outputFile.isOpen()) {
        //Create the block header
            uint16_t *shead;
            shead=( u_int16_t *) calloc(16, sizeof(u_int16_t *));
            shead[0]= 16;
            shead[1]= 0;
            shead[2]= fileNumber;
            shead[3]= 18264;
            shead[4]= 16;
            //Write the block header
            output.writeRawData( (char *) shead, 32);
            //Write the cache
            output.writeRawData( (char *) cache, 16352);
            return 0;
    }
    else  {
        return 1;
    }
}

