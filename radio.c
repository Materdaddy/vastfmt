#include "logging.h"
#include "fmtx.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int ret;
    char cpuid[32];
    char rev[32];
	double dVal;

	// Parse our frequency
	if ( argc < 2 )
	{
		printf("You didn't give me a parameter I like\n");
		return -1;
	}
	dVal = strtod(argv[1], NULL);

	// Setup extern variables
    fmtxCmdSpecial = FMTX_SPECIAL_FRONTEND; // We want to work with frontend to set frequency
	loglevel = LOG_ERROR; // Turn down our logging

	// Set string terminators before calling fmtx
    cpuid[0] = '\0';
    rev[0] = '\0';

    ret = fmtxIoAppIdString(cpuid, rev);
    if (ret != FMTX_MODE_OK)
	{
        printf("Can't get id string!\n");
        return -1;
    }
    printf("Got CPU ID: %s rev.: %s\n", cpuid, rev);

	// Set our frequency
    if(fmtxTransmitterSetTuneFreq(dVal) != FMTX_MODE_OK){
		printf("Couldn't set freq to %d\n", dVal);
		return -1;
	}
	printf("Set frequency to %.1lf MHz\n", dVal);

	return 0;
}

#ifdef PORT_THE_REST_OF_THIS

int MainWindow::SetFrontendPowerUp() {
    int ret;
    qDebug() << __FUNCTION__;
    querier->HidAccessMutex.lock();
    ret = fmtxIoAppFeUp();
    querier->HidAccessMutex.unlock();
    switch(ret){
    case FMTX_MODE_POWER_UP:
        qDebug() << "Frontend powered up!";
        SetTransmitIcon(ICON_NOTX);
        fmtxCmdStatus=FMTX_MODE_OK;
        break;
    case FMTX_MODE_TRANSMITTING:
        qDebug() << "Frontend already powered up!";
        SetTransmitIcon(ICON_TX);
        fmtxCmdStatus=FMTX_MODE_OK;
        break;
    default:
        qDebug() << "Error while powering up";
        fmtxCmdStatus=FMTX_MODE_NONE;
        break;
    }

    if(ret == FMTX_MODE_POWER_UP || ret == FMTX_MODE_TRANSMITTING) {
        querier->is_ready=true;
        liveEditingEnabled=ui->checkBox_sw_live_edit->isChecked();
        ui->actionRead_from_Device->setEnabled(true);
        ui->actionWrite_to_Device->setEnabled(true);
    }

    return ret;
}

void MainWindow::SetFrontendReset() {
    qDebug() << __FUNCTION__;
    if(fmtxIoAppFeReset() != FMTX_MODE_POWER_DOWN)
        qDebug() << "Error while resetting frontend";
    querier->is_ready=false;
    liveEditingEnabled=false;
    SetTransmitIcon(ICON_WARN);
    ui->actionRead_from_Device->setEnabled(false);
    ui->actionWrite_to_Device->setEnabled(false);
    fmtxCmdStatus=FMTX_MODE_NONE;
}

bool MainWindow::SetAudioCompLimFlags()
{
    uint8_t CompLimFlags = 0;
    bool ret = true;
    if(!liveEditingEnabled)
        return false;
    if(ui->checkBox_audio_enablecompressor->isChecked())
        CompLimFlags |= TX_ACOMP_ENABLE_ACEN_MASK;
    if(ui->checkBox_audioLimiterEnable->isChecked())
        CompLimFlags |= TX_ACOMP_ENABLE_LIMITEN_MASK;
    querier->HidAccessMutex.lock();
    if(fmtxAudioSetAcompFlags(CompLimFlags) != FMTX_MODE_OK){
        qDebug() << "Can't set compressor/limiter flag";
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

bool MainWindow::SetAudioCompressorThreshold(int iVal)
{
    bool ret = true;
    if(!liveEditingEnabled)
        return false;
    querier->HidAccessMutex.lock();
    if(fmtxAudioSetAcompThreshold(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set audio compressor threshold";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

bool MainWindow::SetAudioCompressorGain(int iVal)
{
    bool ret = true;
    if(!liveEditingEnabled)
        return false;
    querier->HidAccessMutex.lock();
    if(fmtxAudioSetAcompGain(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set audio compressor gain";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

bool MainWindow::SetAudioCompressorReleaseTime(int iVal)
{
    bool ret = true;
    if(!liveEditingEnabled)
        return false;
    querier->HidAccessMutex.lock();
    if(fmtxAudioSetAcompReleaseTimeId(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set audio compressor release time";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

bool MainWindow::SetAudioCompressorAttackTime(double dVal)
{
    bool ret = true;
    if(!liveEditingEnabled)
        return false;
    querier->HidAccessMutex.lock();
    if(fmtxAudioSetAcompAttackTime(dVal) != FMTX_MODE_OK){
        qDebug() << "Can't set audio compressor attack time";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

bool MainWindow::SetAudioLimiterReleaseTime(double dVal)
{
    bool ret = true;
    int new_div;

    //prevent division by zero
    if(dVal < 0.256)
        dVal = 0.256;

    new_div = 512.0/dVal;
    //take care about 5th digitsa after point that cutted
    if((audioLimiter_prev_div == new_div) &&
            (512.0/audioLimiter_prev_div - dVal > 0.0001))
        new_div++;

    if(new_div < 5)
        new_div=5;
    if(new_div > 2000)
        new_div=2000;

    audioLimiter_prev_div = new_div;
    qDebug() << __FUNCTION__ << "set_div: " << audioLimiter_prev_div
             << " val:" << 512.0/audioLimiter_prev_div;
    ui->doubleSpinBox_audio_lim_release_time->blockSignals(true);
    ui->doubleSpinBox_audio_lim_release_time->setValue(512.0/audioLimiter_prev_div);
    ui->doubleSpinBox_audio_lim_release_time->blockSignals(false);

    if(!liveEditingEnabled)
        return false;
    querier->HidAccessMutex.lock();
    if(fmtxAudioSetLimiterReleaseTime(512.0/audioLimiter_prev_div) != FMTX_MODE_OK){
        qDebug() << "Can't set audio limiter release time";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

bool MainWindow::SetAudioASQFlags()
{
    bool ret = true;
    uint8_t ASQFlags = 0;
    if(!liveEditingEnabled)
        return false;
    if(ui->checkBox_asq_overmoddetect->isChecked())
        ASQFlags |= TX_ASQ_INT_SELECT_OVERMODIEN_MASK;
    if(ui->checkBox_asq_detectLow->isChecked())
        ASQFlags |= TX_ASQ_INT_SELECT_IALLIEN_MASK;
    if(ui->checkBox_asq_detectHigh->isChecked())
        ASQFlags |= TX_ASQ_INT_SELECT_IALHIEN_MASK;
    querier->HidAccessMutex.lock();
    if(fmtxAudiosetAsqIntFlags(ASQFlags) != FMTX_MODE_OK){
        qDebug() << "Can't set ASQ flags";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

bool MainWindow::SetAudioASQLevelLow(int iVal)
{
    bool ret = true;
    if(!liveEditingEnabled)
        return false;
    querier->HidAccessMutex.lock();
    if(fmtxAudioSetAsqLevelLow(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set ASQ Low Level Threshold";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

bool MainWindow::SetAudioASQDurationLow(int iVal)
{
    bool ret = true;
    if(!liveEditingEnabled)
        return false;
    querier->HidAccessMutex.lock();
    if(fmtxAudioSetAsqDurationLow(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set ASQ Low Duration";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

bool MainWindow::SetAudioASQLevelHigh(int iVal)
{
    bool ret = true;
    if(!liveEditingEnabled)
        return false;
    querier->HidAccessMutex.lock();
    if(fmtxAudioSetAsqLevelHigh(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set ASQ High Level Threshold";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

bool MainWindow::SetAudioASQDurationHigh(int iVal)
{
    bool ret = true;
    if(!liveEditingEnabled)
        return false;
    querier->HidAccessMutex.lock();
    if(fmtxAudioSetAsqDurationHigh(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set ASQ High Duration";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

bool MainWindow::SetDigitalAudioHack()
{
    bool ret = true;
    if(!liveEditingEnabled)
        return false;
    querier->HidAccessMutex.lock();
    if(fmtxIoAppEnableAudio() != FMTX_MODE_OK){
        qDebug() << "Can't set Digital audio hack";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

bool MainWindow::SetAudioStandAlone(int iVal)
{
    int iVal1 = ui->comboBox_audio_input_level->currentIndex();
    switch(iVal){
    case 0:
        ui->spinBox_audio_input_amplitude->setValue(190);
        break;
    case 1:
        ui->spinBox_audio_input_amplitude->setValue(301);
        break;
    case 2:
        ui->spinBox_audio_input_amplitude->setValue(416);
        break;
    case 3:
        ui->spinBox_audio_input_amplitude->setValue(636);
        break;
    default:
        break;
    }

    int iVal2 = ui->spinBox_audio_input_amplitude->value();
    bool ret = true;
    if(!liveEditingEnabled)
        return false;
    querier->HidAccessMutex.lock();
    if(fmtxAudioSetInputLevels(iVal1, iVal2) != FMTX_MODE_OK){
        qDebug() << "Can't set ASQ High Duration";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

bool MainWindow::SetAudioStandAloneMute(int val)
{
    bool ret = true;
    if(!liveEditingEnabled)
        return false;
    querier->HidAccessMutex.lock();
    if(fmtxAudioSetInputMute(val) != FMTX_MODE_OK){
        qDebug() << "Can't set ASQ High Duration";
        SetTransmitIcon(ICON_WARN);
        ret = false;
    }
    querier->HidAccessMutex.unlock();
    return ret;
}

void MainWindow::SaveEEPROMParams()
{
    int ret;
    qDebug() << __FUNCTION__;

    ret = QMessageBox::question ( this, tr("Are you sure?"),
                                      tr("Do you really want to write EEPROM config?\n"
                                      "Bad configuration may make frontend inoperable!"),
               QMessageBox::Yes|QMessageBox::Abort, QMessageBox::Abort);

    if(ret != QMessageBox::Yes) {
        qDebug() << "Aborted";
        return;
    }

    bool is_ready_save = querier->is_ready;
    bool liveEditingEnabled_save = liveEditingEnabled;
    querier->is_ready=false;
    liveEditingEnabled=true;
    fmtxCmdSpecial = FMTX_SPECIAL_EEPROM;   ///< now work with eeprom instead of frontend
    fmtxEEPROMInfoClean();
    SetFrontendParams();
    ret = fmtxEEPROMWriteConfig(ui->checkBox_hw_run_txAtPowerUp->isChecked());
    fmtxCmdSpecial = FMTX_SPECIAL_FRONTEND;
    liveEditingEnabled = liveEditingEnabled_save;
    querier->is_ready=is_ready_save;

    if(ret != FMTX_MODE_OK){
        qDebug() << __FUNCTION__ << " write error!";
        QMessageBox::warning(this, tr("Can't write EEPROM"), tr("Error while writing EEPROM Config!"));
        return;
    }

    qDebug() << __FUNCTION__ << "saved!";
    QMessageBox::information(this, tr("Write successed"), tr("EEPROM Config written successfuly!\n"
                                                             "You may restart device to run\n"
                                                             "configuration from EEPROM."));
}

void MainWindow::ShowEEPROMParams()
{
    qDebug() << __FUNCTION__;
    querier->HidAccessMutex.lock();
    fmtxCmdSpecial = FMTX_SPECIAL_EEPROM;   ///< now work with eeprom instead of frontend
    GetFrontendParams();
    fmtxCmdSpecial = FMTX_SPECIAL_FRONTEND;
    querier->HidAccessMutex.unlock();
}

void MainWindow::SetRDSPI()
{
    bool ok=false;
    uint16_t u16Val;
    if(!liveEditingEnabled)
        return;
    u16Val=ui->lineEdit_rds_progid->text().toUShort(&ok, 16);
    if(!ok) {
        qDebug() << "Can't set RDS PI: \"" << ui->lineEdit_rds_progid->text() << "\" - bad value";
    } else {
        querier->HidAccessMutex.lock();
        if(fmtxRDSSetPI(u16Val) != FMTX_MODE_OK){
            qDebug() << "Can't set RDS AF";
            SetTransmitIcon(ICON_WARN);
        }
        querier->HidAccessMutex.unlock();
    }
}

void MainWindow::SetRDSPsMiscFlags()
{
    uint16_t MiscFlags = 0;
    if(!liveEditingEnabled)
        return;
    if(ui->checkBox_rds_dynPT->isChecked())
        MiscFlags |= TX_RDS_PS_MISC_RDSD3_MASK;
    if(ui->checkBox_rds_compr_code->isChecked())
        MiscFlags |= TX_RDS_PS_MISC_RDSD2_MASK;
    if(ui->checkBox_rds_art_head->isChecked())
        MiscFlags |= TX_RDS_PS_MISC_RDSD1_MASK;
    if(ui->checkBox_rds_is_stereo->isChecked())
        MiscFlags |= TX_RDS_PS_MISC_RDSD0_MASK;
    if(ui->checkBox_rds_forceb->isChecked())
        MiscFlags |= TX_RDS_PS_MISC_FORCEB_MASK;
    if(ui->radioButton_rds_music->isChecked())
        MiscFlags |= TX_RDS_PS_MISC_RDSMS_MASK;

    MiscFlags |= TX_RDS_PS_MISC_RDSPTY_MASK & (ui->comboBox_rds_pty->currentIndex() << TX_RDS_PS_MISC_RDSPTY_SHFT);

    if(ui->checkBox_rds_ta->isChecked())
        MiscFlags |= TX_RDS_PS_MISC_RDSTA_MASK;
    if(ui->checkBox_rds_ta_tp->isChecked())
        MiscFlags |= TX_RDS_PS_MISC_RDSTP_MASK;
    querier->HidAccessMutex.lock();
    if(fmtxRDSSetPsMiscFlags(MiscFlags) != FMTX_MODE_OK){
        qDebug() << "Can't set RDS Misc flag";
    }
    querier->HidAccessMutex.unlock();
}

void MainWindow::SetRDSAF(double dVal)
{
    if(!liveEditingEnabled)
        return;
    querier->HidAccessMutex.lock();
    if(fmtxRDSSetPsAF(dVal, ui->checkBox_rds_useaf->isChecked()) != FMTX_MODE_OK){
        qDebug() << "Can't set RDS AF";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
}

void MainWindow::SetRDSFIFO(int iVal)
{
    if(!liveEditingEnabled)
        return;
    if(!ui->checkBox_rds_fifo->isChecked())
        iVal=0;
    querier->HidAccessMutex.lock();
    if(fmtxRDSSetFifoSize(iVal)!= FMTX_MODE_OK){
        qDebug() << "Can't set RDS FIFO";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
}

void MainWindow::SetRDSPSNum(int iVal)
{
    if(!liveEditingEnabled)
        return;
    querier->HidAccessMutex.lock();
    if(fmtxRDSSetPsMessageCount(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set RDS PS Number";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
}

void MainWindow::SetRDSPSPercentage(int iVal)
{
    if(!liveEditingEnabled)
        return;
    querier->HidAccessMutex.lock();
    if(fmtxRDSSetPsMixId(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set RDS PS Percentage";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
}

void MainWindow::SetRDSPSRepeats(int iVal)
{
    if(!liveEditingEnabled)
        return;
    querier->HidAccessMutex.lock();
    if(fmtxRDSSetPsRepeatCount(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set RDS PS Repeats";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
}

int MainWindow::SetRDSPSMessage(int id)
{
    QLineEdit *msg=NULL;
    int ret;
    if(!liveEditingEnabled)
        return FMTX_MODE_NONE;
    msg=GetRDSLineEditById(id);
    querier->HidAccessMutex.lock();
    ret = fmtxRDSSetPsMessageById(id, (char *) msg->text().toStdString().c_str());
    if(ret != FMTX_MODE_OK){
        qDebug() << "Can't set RDS PS Message";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
    rtplus_text="";
    return ret;
}

int MainWindow::SetRDSRT(QString *text_in)
{
    QString text("");
    int ret=0;

    text.append(text_in);
    while(text.length() > 0 && text.endsWith(" "))
        text.chop(1);

    if(!liveEditingEnabled) {
        //return FMTX_MODE_NONE;
    } else {
        querier->HidAccessMutex.lock();
        ret = fmtxRDSSetRtMessage((char *) text.toStdString().c_str());
        if(ret != FMTX_MODE_OK){
            qDebug() << "Can't set RDS RT Message";
            SetTransmitIcon(ICON_WARN);
        }
        querier->HidAccessMutex.unlock();
    }

    /* check if we need RT+ */
    if(ui->checkBox_rds_title_rtplus->isChecked()) {
        QList<int> itemsList;
        int retry_id=0;
        /* song title in the 1st place due it is longer than artist name (in mot cases) */
        querier->parse_rtplus_items(text, querier->rdsRTPlusContent1RegExp, querier->rdsRTPlusContent1Type, itemsList);
        querier->parse_rtplus_items(text, querier->rdsRTPlusContent2RegExp, querier->rdsRTPlusContent2Type, itemsList);
        if(itemsList.size() == 0) {
            querier->parse_rtplus_items(text, querier->rdsRTPlusContent3RegExp, querier->rdsRTPlusContent3Type, itemsList);
            querier->parse_rtplus_items(text, querier->rdsRTPlusContent4RegExp, querier->rdsRTPlusContent4Type, itemsList);
            retry_id++;
        }

        if(itemsList.size()!=0 || !querier->rdsRTPlusDisItem) {
            if(itemsList.size() == 0) itemsList << 0;
            while(itemsList.size() % 6)
                itemsList << 0;
            if(!liveEditingEnabled) {
                updateRDSRTPlusMark(itemsList, retry_id);
            } else {
                querier->HidAccessMutex.lock();
                ret = fmtxRDSSetRtPlusInfo(itemsList.at(0), itemsList.at(1), itemsList.at(2),
                                         itemsList.at(3), itemsList.at(4), itemsList.at(5));
                querier->HidAccessMutex.unlock();
                if(ret != FMTX_MODE_OK){
                    qDebug() << "Can't set RDS RT Message";
                    SetTransmitIcon(ICON_WARN);
                } else {
                    updateRDSRTPlusMark(itemsList, retry_id);
                }
            }
        }
    }
    return ret;
}

void MainWindow::UpdateRDSRTPlusEnabled()
{
    bool enabled = ui->checkBox_rds_title_rtplus->isChecked();
    ui->frame_rds_title_rtplus->setVisible(enabled);
    if(enabled) {
        querier->rdsRTPlusContent1RegExp.setPattern(ui->lineEdit_rds_content1_reg->text());
        querier->rdsRTPlusContent2RegExp.setPattern(ui->lineEdit_rds_content2_reg->text());
        querier->rdsRTPlusContent3RegExp.setPattern(ui->lineEdit_rds_content3_reg->text());
        querier->rdsRTPlusContent4RegExp.setPattern(ui->lineEdit_rds_content4_reg->text());
        querier->rdsRTPlusContent1Type = ui->comboBox_rds_content1_type->currentIndex();
        querier->rdsRTPlusContent2Type = ui->comboBox_rds_content2_type->currentIndex();
        querier->rdsRTPlusContent3Type = ui->comboBox_rds_content3_type->currentIndex();
        querier->rdsRTPlusContent4Type = ui->comboBox_rds_content4_type->currentIndex();
        querier->rdsRTPlusDisItem = ui->checkBox_rds_dis_item->isChecked();
    } else {
        querier->rdsRTPlusContent1RegExp.setPattern("");
        querier->rdsRTPlusContent2RegExp.setPattern("");
        querier->rdsRTPlusContent3RegExp.setPattern("");
        querier->rdsRTPlusContent4RegExp.setPattern("");
        querier->rdsRTPlusContent1Type = RTPLUS_DUMMY_CLASS;
        querier->rdsRTPlusContent2Type = RTPLUS_DUMMY_CLASS;
        querier->rdsRTPlusContent3Type = RTPLUS_DUMMY_CLASS;
        querier->rdsRTPlusContent4Type = RTPLUS_DUMMY_CLASS;
    }
}

/*
Send customized RDS message. Block A setted based on RDS PI-code,\n
other blocks: B,C and D - are avaliable to modify, also you can send\n
message to circular buffer instead of fifo to send it periodically.\n
To send message only once (using fifo buffer), as prefix write "F:",\n
for example: "F:301600004BD7". To send message periodically in\n
circular buffer, write "C:" as prefix, for example: "C:B00824A02005".\n
To Flush circular buffer write "R" symbol before "C:", for example:\n
"RC:B00824A02005" - flushes circular buffer and write first group there\n
Note circular buffe may be cleaned only when re-sending RadioText message.
F:301600004BD7 - send to fifo
C:B00824A02005 - send to circular
RC:B00824A02005 - flush circular and send to circular newer values.
*/
void MainWindow::SetRDSCustomGroup(QString *text)
{
    if(!text)
        text= new QString(ui->lineEdit_rds_custom->text());
    qDebug() << "[MAIN]" << " setting custom group: " << *text;
    if(!liveEditingEnabled)
        return;
    querier->HidAccessMutex.lock();

    if(text->startsWith('R')) {
        if(fmtxRDSSendCustomGroup(TX_RDS_BUFF_IN_MTBUFF, 0, 0, 0, 0, 0, 0) != FMTX_MODE_OK){
            qDebug() << "Can't set RDS Clear Message";
            SetTransmitIcon(ICON_WARN);
            fmtxCmdStatus=FMTX_MODE_NONE;
            return;
        }
        text->remove(0, 1);
    }

    uint8_t flag = TX_RDS_BUFF_IN_LDBUFF;
    if(text->startsWith('F')) {
        flag = flag | TX_RDS_BUFF_IN_FIFO;
    }
    text->remove(0, 2);
    uint8_t B0 = text->mid(0,2).toInt(0, 16);
    uint8_t B1 = text->mid(2,2).toInt(0, 16);
    uint8_t C0 = text->mid(4,2).toInt(0, 16);
    uint8_t C1 = text->mid(6,2).toInt(0, 16);
    uint8_t D0 = text->mid(8,2).toInt(0, 16);
    uint8_t D1 = text->mid(10,2).toInt(0, 16);

    if(fmtxRDSSendCustomGroup(flag, B0, B1, C0, C1, D0, D1) != FMTX_MODE_OK){
        qDebug() << "Can't set RDS Custom Message";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
}

void MainWindow::ConfigureTransmitterEditor(void)
{
    int ret;

    qDebug() << "Setting Transmitter info...";

    SetTransmitPreemphasis(ui->comboBox_transmitter_preemph->currentIndex());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_transmitter_err;

    SetTransmitAudioDeviation(ui->spinBox_transmitter_audioDev->value());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_transmitter_err;

    SetTransmitPilotDeviation(ui->spinBox_transmitter_pilot_devi->value());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_transmitter_err;

    SetTransmitPilotFrequency(ui->spinBox_transmitter_pilot_freq->value());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_transmitter_err;

    SetTransmitRDSDeviation(ui->spinBox_transmitter_rds_devi->value());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_transmitter_err;

    SetTransmitFrequency(ui->doubleSpinBox_transmitter_freq->value());
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_transmitter_err;

    querier->HidAccessMutex.lock();
    if(ui->checkBox_transmitter_enabled->isChecked()) {
        if(ui->checkBox_transmitter_antcap->isChecked())
            ret=fmtxTransmitterSetTunePower(ui->spinBox_transmitter_power->value(), ui->doubleSpinBox_transmitter_antcap_val->value());
        else
            ret=fmtxTransmitterSetTunePower(ui->spinBox_transmitter_power->value(), 0.0);
        SetTransmitIcon(ICON_TX);
    } else {
        ret=fmtxTransmitterSetTunePower(0, 0.0);
        SetTransmitIcon(ICON_NOTX);
    }
    querier->HidAccessMutex.unlock();
    if(ret != FMTX_MODE_OK) {
        qDebug() << "Can't enable transmission";
        SetTransmitIcon(ICON_WARN);
        goto configure_transmitter_err;
    }

    SetTransmitComponentFlags();
    if(fmtxCmdStatus!=FMTX_MODE_OK) goto configure_transmitter_err;

    qDebug() << "Transmitter info sended";
    return;

configure_transmitter_err:
    qDebug() << "Error while sending transmitter info";
}

void MainWindow::SetTransmitPreemphasis(int iVal)
{
    if(!liveEditingEnabled)
        return;
    querier->HidAccessMutex.lock();
    if(fmtxTransmitterSetPreemphasisId(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set transmitter preemphasis flag";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
}

void MainWindow::SetTransmitAudioDeviation(int iVal)
{
    if(!liveEditingEnabled)
        return;
    querier->HidAccessMutex.lock();
    if(fmtxTransmitterSetAudioDeviation(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set transmitter audio deviation flag";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
}

void MainWindow::SetTransmitComponentFlags()
{
    uint8_t ComponentFlags = 0;
    if(!liveEditingEnabled)
        return;
    if(ui->checkBox_transmitter_lmr->isChecked())
        ComponentFlags |= TX_COMPONENT_ENABLE_LMR_MASK;
    if(ui->checkBox_transmitter_pilot->isChecked())
        ComponentFlags |= TX_COMPONENT_ENABLE_PILOT_MASK;
    if(ui->checkBox_transmitter_rds->isChecked())
        ComponentFlags |= TX_COMPONENT_ENABLE_RDS_MASK;
    querier->HidAccessMutex.lock();
    if(fmtxTransmitterSetComponentFlags(ComponentFlags) != FMTX_MODE_OK){
        qDebug() << "Can't set transmitter components flag";
        SetTransmitIcon(ICON_WARN);
    }

    querier->HidAccessMutex.unlock();
}

void MainWindow::SetTransmitPilotDeviation(int iVal)
{
    if(!liveEditingEnabled)
        return;
    querier->HidAccessMutex.lock();
    if(fmtxTransmitterSetPilotDeviation(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set transmitter pilot deviation flag";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
}

void MainWindow::SetTransmitPilotFrequency(int iVal)
{
    if(!liveEditingEnabled)
        return;
    querier->HidAccessMutex.lock();
    if(fmtxTransmitterSetPilotFrequency(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set transmitter pilot frequency flag";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
}

void MainWindow::SetTransmitRDSDeviation(int iVal)
{
    if(!liveEditingEnabled)
        return;
    querier->HidAccessMutex.lock();
    if(fmtxTransmitterSetRDSDeviation(iVal) != FMTX_MODE_OK){
        qDebug() << "Can't set transmitter audio deviation flag";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
}

void MainWindow::SetTransmitFrequency(double dVal)
{
    if(!liveEditingEnabled)
        return;
    querier->HidAccessMutex.lock();
    if(fmtxTransmitterSetTuneFreq(dVal) != FMTX_MODE_OK){
        qDebug() << "Can't set transmitter audio deviation flag";
        SetTransmitIcon(ICON_WARN);
    }
    querier->HidAccessMutex.unlock();
}

/* sets power and antcap at the same time */
int MainWindow::SetTransmitPower()
{
    uint8_t ret;
    ui->label_transmitter_level->setText(QString("%1")
                                         .arg(ui->spinBox_transmitter_power->value()));
    if(!liveEditingEnabled)
        return FMTX_MODE_NONE;
    querier->HidAccessMutex.lock();
    if(ui->checkBox_transmitter_antcap->isChecked()) {
        ret=fmtxTransmitterSetTunePower(ui->spinBox_transmitter_power->value(), ui->doubleSpinBox_transmitter_antcap_val->value());
    } else {
        ret=fmtxTransmitterSetTunePower(ui->spinBox_transmitter_power->value(), 0.0);
    }
    if(ret != FMTX_MODE_OK){
        qDebug() << "Can't set transmitter power and capacitor";
        SetTransmitIcon(ICON_WARN);
    } else
    querier->HidAccessMutex.unlock();

    querier->timer.setInterval(2000);
    return ret;
}

void MainWindow::SetTransmitEnabled( bool bEnabled )
{
    uint8_t ret;
    if(!liveEditingEnabled)
        return;

    qDebug() << QString("Transmission: %1").arg(bEnabled?"enabled":"disabled");

    if(bEnabled) {
        ret = SetTransmitPower();
        SetTransmitIcon(ICON_TX);
    } else {
        querier->HidAccessMutex.lock();
        ret=fmtxTransmitterSetTunePower(0, 0.0);
        querier->HidAccessMutex.unlock();
        querier->timer.setInterval(2000);
        SetTransmitIcon(ICON_NOTX);
    }

    if(ret != FMTX_MODE_OK){
        qDebug() << "Can't enable transmission";
        SetTransmitIcon(ICON_WARN);
    }
}

#endif //PORT_THE_REST_OF_THIS
