// Translated by: Fabio Olimpieri <fabio.olimpieri@tin.it>
// Updated by: Roberto Togni <see AUTHORS for email address>

// Updated to help_mp-en.h v1.123

// ========================= MPlayer help ===========================

#ifdef HELP_MP_DEFINE_STATIC
static char help_text[]=
"Uso:   mplayer [opzioni] [percorso/]nome_file\n"
"\n"
"Opzioni di base: (vedi la pagina man per la lista completa)\n"
" -vo <drv[:dev]> sceglie driver e dispositivo output video ('-vo help' lista)\n"
" -ao <drv[:dev]> sceglie driver e dispositivo output audio ('-ao help' lista)\n"
#ifdef HAVE_VCD
" vcd://<trackno>  legge traccia VCD (Video CD) da dispositivo anzich? da file\n"
#endif
#ifdef USE_DVDREAD
" dvd://<titleno>  legge titolo/traccia DVD dal dispositivo anzich? da file\n"
" -alang/-slang   sceglie lingua audio/sottotitoli DVD (cod nazione 2 caratteri)\n"
#endif
" -ss <timepos>   cerca una determinata posizione (in secondi o in hh:mm:ss) \n"
" -nosound        non riproduce l\'audio\n"
" -fs -vm -zoom   opzioni schermo intero (schermo int,modo video,scalatura sw)\n"
" -x <x> -y <y>   imposta la risoluzione dello schermo (usare con -vm o -zoom)\n"
" -sub <file>     file sottotitoli da usare (vedi anche -subfps, -subdelay)\n"
" -playlist <file> specifica il file della playlist\n"
" -vid x -aid y   seleziona il flusso video (x) ed audio (y) da riprodurre\n"
" -fps x -srate y cambia il rate del video (x fps) e dell\'audio (y Hz)\n"
" -pp <quality>   abilita filtro postelaborazione (vedi pagina man per dettagli)\n"
" -framedrop      abilita lo scarto dei fotogrammi (per macchine lente)\n"
"\n"
"Tasti principali: (vedi pagina man per lista, controlla anche input.conf)\n"
" <-  o  ->       va indietro/avanti di 10 secondi\n"
" su o gi?        va indietro/avanti di 1 minuto\n"
" pagsu o paggi?  va indietro/avanti di 10 minuti\n"
" < o >           va indietro/avanti nella playlist\n"
" p o SPAZIO      pausa (premere un qualunque tasto per continuare)\n"
" q o ESC         ferma la riproduzione ed esce dal programma\n"
" + o -           regola il ritardo audio di +/- 0.1 secondi\n"
" o               modalit? OSD: niente / barra ricerca / barra ricerca + tempo\n"
" * o /           incrementa o decrementa il volume pcm\n"
" z o x           regola il ritardo dei sottotitoli di +/- 0.1 secondi\n"
" r o t           posizione alto/basso dei sottotitoli, vedi anche -vf expand\n"
"\n"
" * * * VEDI PAGINA MAN PER DETTAGLI, ULTERIORI OPZIONI AVANZATE E TASTI! * * *\n"
"\n";
#endif

// ========================= MPlayer messages ===========================

// mplayer.c:

#define MSGTR_Exiting "\nIn uscita...\n"
#define MSGTR_ExitingHow "\nIn uscita... (%s)\n"
#define MSGTR_Exit_quit "Uscita"
#define MSGTR_Exit_eof "Fine del file"
#define MSGTR_Exit_error "Errore fatale"
#define MSGTR_IntBySignal "\nMPlayer interrotto dal segnale %d nel modulo: %s \n"
#define MSGTR_NoHomeDir "Impossibile trovare la HOME directory\n"
#define MSGTR_GetpathProblem "Problema in get_path(\"config\")\n"
#define MSGTR_CreatingCfgFile "Creo il file di configurazione: %s\n"
#define MSGTR_InvalidAOdriver "Nome del diver audio di output non valido: %s\nUsa '-ao help' per avere una lista dei driver audio disponibili.\n"
#define MSGTR_CopyCodecsConf "(copia/collega etc/codecs.conf (dall\'albero dei sorgenti di MPlayer) a ~/.mplayer/codecs.conf)\n"
#define MSGTR_BuiltinCodecsConf "Utilizzo la versione interna predefinita di codecs.conf\n"
#define MSGTR_CantLoadFont "Impossibile caricare i font: %s\n"
#define MSGTR_CantLoadSub "Impossibile caricare i sottotitoli: %s\n"
#define MSGTR_DumpSelectedStreamMissing "dump: FATAL: manca il flusso selezionato!\n"
#define MSGTR_CantOpenDumpfile "Impossibile aprire il file di dump!!!\n"
#define MSGTR_CoreDumped "core dumped :)\n"
#define MSGTR_FPSnotspecified "FPS non specificato (o non valido) nell\'intestazione! Usa l\'opzione -fps!\n"
#define MSGTR_TryForceAudioFmtStr "Cerco di forzare l\'uso della famiglia dei driver dei codec audio %d...\n"
#define MSGTR_CantFindAudioCodec "Impossibile trovare il codec per il formato audio 0x%X!\n"
#define MSGTR_RTFMCodecs "Leggi DOCS/it/codecs.html!\n"
#define MSGTR_TryForceVideoFmtStr "Cerco di forzare l\'uso della famiglia dei driver dei codec video %d...\n"
#define MSGTR_CantFindVideoCodec "Impossibile trovare il codec per il formato video 0x%X!\n"
#define MSGTR_VOincompCodec "Mi dispiace, il dispositivo di video_out selezionato ? incompatibile con questo codec.\n"
#define MSGTR_CannotInitVO "FATAL: Impossibile inizializzare il driver video!\n"
#define MSGTR_CannotInitAO "Impossibile aprire/inizializzare il dispositivo audio -> NESSUN SUONO\n"
#define MSGTR_StartPlaying "Inizio la riproduzione...\n"

#define MSGTR_SystemTooSlow "\n\n"\
"         ***************************************************************\n"\
"         **** Il tuo sistema ? troppo lento per questa riproduzione! ***\n"\
"         ***************************************************************\n"\
"!!! Possibili cause, problemi, soluzioni: \n"\
"- Nella maggior parte dei casi: driver audio corrotto/bacato. Soluzione: prova -ao sdl o usa\n"\
"  ALSA 0.5 o l\'emulazione oss di ALSA 0.9. Leggi DOCS/it/sound.html per ulteriori suggerimenti!\n"\
" Puoi anche provare con -autosync 30 o altri valori.\n"\
"- Output video lento. Prova un differente -vo driver (per la lista completa: -vo help) o prova\n"\
"  con -framedrop!  Leggi DOCS/it/video.html per suggerimenti sulla regolazione/accelerazione del video.\n"\
"- Cpu lenta. Non provare a riprodurre grossi dvd/divx su cpu lente! Prova -hardframedrop\n"\
"- File corrotto. Prova varie combinazioni di: -nobps  -ni  -mc 0  -forceidx\n"\
"- Per riprodurre da dispositivi lenti (dischi nfs/smb, dvd, vcd etc) prova -cache 8192\n"\
"- Stai usando -cache per riprodurre un file AVI senza interleave? Prova con -nocache.\n"\
"Se il problema non ? in nessuno di questi casi, allora leggi DOCS/it/bugreports.html!\n\n"

#define MSGTR_NoGui "MPlayer ? stato compilato senza il supporto della GUI!\n"
#define MSGTR_GuiNeedsX "LA GUI di MPlayer richiede X11!\n"
#define MSGTR_Playing "In riproduzione %s\n"
#define MSGTR_NoSound "Audio: nessun suono!!!\n"
#define MSGTR_FPSforced "FPS forzato a %5.3f  (ftime: %5.3f)\n"
#define MSGTR_CompiledWithRuntimeDetection "Complato con riconoscimento run time della cpu - attenzione, non ? il metodo ideale! Per avere le migliori prestazioni, ricompila MPlayer con --disable-runtime-cpudetection\n"
#define MSGTR_CompiledWithCPUExtensions "Compilato per CPU x86 con estensioni:"
#define MSGTR_AvailableVideoOutputDrivers "Driver di output video disponibili:\n"
#define MSGTR_AvailableAudioOutputDrivers "Driver di output audio disponibili:\n"
#define MSGTR_AvailableAudioCodecs "Codec audio disponibili:\n"
#define MSGTR_AvailableVideoCodecs "Codec video disponibili:\n"
#define MSGTR_AvailableAudioFm "\nFamiglie/driver di codec audio disponibili (compilati):\n"
#define MSGTR_AvailableVideoFm "\nFamiglie/driver di codec video disponibili (compilati):\n"
#define MSGTR_AvailableFsType "Modi disponibili a schermo pieno:\n"
#define MSGTR_UsingRTCTiming "Sto utilizzando la temporizzazione hardware RTC di Linux (%ldHz)\n"
#define MSGTR_CannotReadVideoProperties "Video: impossibile leggere le propriet?\n"
#define MSGTR_NoStreamFound "Nessun flusso trovato\n"
#define MSGTR_ErrorInitializingVODevice "Errore aprendo/inizializzando il dispositivo di output video (-vo) selezionato!\n"
#define MSGTR_ForcedVideoCodec "Codec video forzato: %s\n"
#define MSGTR_ForcedAudioCodec "Codec audio forzato: %s\n"
#define MSGTR_Video_NoVideo "Video: no video!!!\n"
#define MSGTR_NotInitializeVOPorVO "\nFATAL: Impossibile inizializzare i filtri video (-vf) o l'output video (-vo)!\n"
#define MSGTR_Paused "\n  =====  PAUSA  =====\r"
#define MSGTR_PlaylistLoadUnable "\nImpossibile caricare la playlist %s\n"
#define MSGTR_Exit_SIGILL_RTCpuSel \
"- MPlayer ? stato interrotto dal segnale 'Istruzione illegale'.\n"\
"  Potrebbe essere un errore nel codice di rilevazione del tipo di processore...\n"\
"  leggi DOCS/it/bugreports.html\n"
#define MSGTR_Exit_SIGILL \
"- MPlayer ? stato interrotto dal segnale 'Istruzione illegale'.\n"\
"  Solitamente questo avviene quando si esegue il programma su un processore\n"\
"  diverso da quello per cui ? stato compilato/ottimizzato. Verificalo!"
#define MSGTR_Exit_SIGSEGV_SIGFPE \
"- MPlayer ? stato interrotto per un errore nell'uso della CPU/FPU/RAM.\n"\
"  Ricompila MPlayer con --enable-debug e crea un baktrace ed un disassemblato\n"\
"  con 'gdb'. Per i dettagli, leggi DOCS/it/bugreports.html#crash\n"
#define MSGTR_Exit_SIGCRASH \
"- MPlayer ? andato in crash. Questo non dovrebbe accadere.\n"\
"  Pu? essere un errore nel codice di MPlayer _o_ nei tuoi driver _o_ nella tua\n"\
"  versione di gcc. Se ritieni sia colpa di MPlayer, leggi DOCS/it/bugreports.html\n"\
"  e segui le istruzioni. Non possiamo aiutarti, e non lo faremo, se non\n"\
"  fornisci queste informazioni quando segnali un possibile problema.\n"

// mencoder.c:

#define MSGTR_UsingPass3ControllFile "Sto usando il file di controllo pass3: %s\n"
#define MSGTR_MissingFilename "\nNome file mancante!\n\n"
#define MSGTR_CannotOpenFile_Device "Impossibile aprire il file/dispositivo\n"
#define MSGTR_CannotOpenDemuxer "Impossibile aprire il demuxer\n"
#define MSGTR_NoAudioEncoderSelected "\nNessun encoder audio (-oac) selezionato! Selezionane uno o usa -nosound. Usa -oac help!\n"
#define MSGTR_NoVideoEncoderSelected "\nNessun encoder video (-ovc) selezionato! Selezionane uno, usa -ovc help!\n"
#define MSGTR_CannotOpenOutputFile "Impossibile aprire il file di output '%s'\n"
#define MSGTR_EncoderOpenFailed "Errore nell'apertura dell'encoder\n"
#define MSGTR_ForcingOutputFourcc "Forza il fourcc in uscita a %x [%.4s]\n"
#define MSGTR_WritingAVIHeader "Scrittura intestazione AVI...\n"
#define MSGTR_DuplicateFrames "\nduplicate %d frame(s)!!!    \n"
#define MSGTR_SkipFrame "\nskip frame!!!    \n"
#define MSGTR_ErrorWritingFile "%s: errore nella scrittura del file.\n"
#define MSGTR_WritingAVIIndex "\nScrittura indice AVI...\n"
#define MSGTR_FixupAVIHeader "Completamento intestazione AVI...\n"
#define MSGTR_RecommendedVideoBitrate "Il bitrate video consigliato per %s CD ?: %d\n"
#define MSGTR_VideoStreamResult "\nFlusso video : %8.3f kbit/s  (%d bps)  dimensione: %d byte  %5.3f secondi  %d frame\n"
#define MSGTR_AudioStreamResult "\nFlusso audio : %8.3f kbit/s  (%d bps)  dimensione: %d byte  %5.3f secondi\n"

// cfg-mencoder.h:

#define MSGTR_MEncoderMP3LameHelp "\n\n"\
" vbr=<0-4>     metodo bitrate variabile\n"\
"                0: cbr\n"\
"                1: mt\n"\
"                2: rh(default)\n"\
"                3: abr\n"\
"                4: mtrh\n"\
"\n"\
" abr           bitrate medio\n"\
"\n"\
" cbr           bitrate costante\n"\
"               Forza il metodo CBR anche sui successivi preset ABR\n"\
"\n"\
" br=<0-1024>   specica il bitrate in kBit (solo CBR e ABR)\n"\
"\n"\
" q=<0-9>       qualit? (0-massima, 9-minima) (solo per VBR)\n"\
"\n"\
" aq=<0-9>      qualit? algoritmo (0-migliore/pi? lento, 9-peggiore/pi? veloce)\n"\
"\n"\
" ratio=<1-100> rapporto di compressione\n"\
"\n"\
" vol=<0-10>    imposta il guadagno dell'ingresso audio\n"\
"\n"\
" mode=<0-3>    (default: auto)\n"\
"                0: stereo\n"\
"                1: joint-stereo\n"\
"                2: due canali indipendenti\n"\
"                3: mono\n"\
"\n"\
" padding=<0-2>\n"\
"                0: no\n"\
"                1: tutto\n"\
"                2: regola\n"\
"\n"\
" fast          attiva la codifica pi? veloce sui successivi preset VBR,\n"\
"               qualit? leggermente inferiore ai bitrate pi? alti.\n"\
"\n"\
" preset=<value> fornisce le migliori impostazioni possibili di qualit?.\n"\
"                 medium: codifica VBR, buona qualit?\n"\
"                 (intervallo bitrate 150-180 kbps)\n"\
"                 standard:  codifica VBR, qualit? alta\n"\
"                 (intervallo bitrate 170-210 kbps)\n"\
"                 extreme: codifica VBR, qualit? molto alta\n"\
"                 (intervallo bitrate 200-240 kbps)\n"\
"                 insane:  codifica CBR, massima qualit? via preset\n"\
"                 (bitrate 320 kbps)\n"\
"                 <8-320>: codifica ABR con bitrate medio impostato in kbps.\n\n"

// open.c, stream.c:
#define MSGTR_CdDevNotfound "Dispositivo CD-ROM '%s' non trovato!\n"
#define MSGTR_ErrTrackSelect "Errore nella selezione della traccia del VCD!"
#define MSGTR_ReadSTDIN "Leggo da stdin...\n"
#define MSGTR_UnableOpenURL "Impossibile aprire la URL: %s\n"
#define MSGTR_ConnToServer "Connesso al server: %s\n"
#define MSGTR_FileNotFound "File non trovato: '%s'\n"

#define MSGTR_SMBInitError "Impossibile inizializzare la libreria libsmbclient: %d\n"
#define MSGTR_SMBFileNotFound "Impossibile aprire dalla rete: '%s'\n"
#define MSGTR_SMBNotCompiled "MPlayer non ? stato compilato con supporto di lettura da SMB\n"

#define MSGTR_CantOpenDVD "Impossibile aprire il dispositivo DVD: %s\n"
#define MSGTR_DVDwait "Leggo la struttura del disco, per favore aspetta...\n"
#define MSGTR_DVDnumTitles "Ci sono %d titoli su questo DVD.\n"
#define MSGTR_DVDinvalidTitle "Numero del titolo del DVD non valido: %d\n"
#define MSGTR_DVDnumChapters "Ci sono %d capitoli in questo titolo del DVD.\n"
#define MSGTR_DVDinvalidChapter "Numero del capitolo del DVD non valido: %d\n"
#define MSGTR_DVDnumAngles "Ci sono %d angolature in questo titolo del DVD.\n"
#define MSGTR_DVDinvalidAngle "Numero delle angolature del DVD non valido: %d\n"
#define MSGTR_DVDnoIFO "Impossibile aprire il file IFO per il titolo del DVD %d.\n"
#define MSGTR_DVDnoVOBs "Impossibile aprire il titolo VOBS (VTS_%02d_1.VOB).\n"
#define MSGTR_DVDopenOk "DVD aperto con successo!\n"

// demuxer.c, demux_*.c:
#define MSGTR_AudioStreamRedefined "Avvertimento! Intestazione del flusso audio %d ridefinito!\n"
#define MSGTR_VideoStreamRedefined "Avvertimento! Intestazione del flusso video %d ridefinito!\n"
#define MSGTR_TooManyAudioInBuffer "\nDEMUXER: Troppi (%d in %d byte) pacchetti audio nel buffer!\n"
#define MSGTR_TooManyVideoInBuffer "\nDEMUXER: Troppi (%d in %d byte) pacchetti video nel buffer!\n"
#define MSGTR_MaybeNI "(forse stai riproducendo un flusso/file senza interleave o il codec non funziona)\n" \
          "Per i file .AVI, prova a forzare la modalit? 'no interleave' con l'opzione -ni.\n"
#define MSGTR_SwitchToNi "\nRielvato file .AVI con interleave errato - passo alla modalit? -ni!\n"
#define MSGTR_Detected_XXX_FileFormat "Rilevato formato file %s!\n"
#define MSGTR_DetectedAudiofile "Rilevato file audio!\n"
#define MSGTR_NotSystemStream "il formato non ? \'MPEG System Stream\'... (forse ? \'Transport Stream\'?)\n"
#define MSGTR_InvalidMPEGES "Flusso MPEG-ES non valido??? Contatta l\'autore, pu? essere un baco :(\n"
#define MSGTR_FormatNotRecognized "===== Mi dispiace, questo formato file non ? riconosciuto/supportato ======\n"\
				  "=== Se questo ? un file AVI, ASF o MPEG, per favore contatta l\'autore! ===\n"
#define MSGTR_MissingVideoStream "Nessun flusso video trovato!\n"
#define MSGTR_MissingAudioStream "Nessun flusso audio trovato...  ->nessun suono\n"
#define MSGTR_MissingVideoStreamBug "Manca il flusso video!? Contatta l\'autore, pu? essere un baco :(\n"

#define MSGTR_DoesntContainSelectedStream "demux: il file non contiene il flusso audio o video selezionato\n"

#define MSGTR_NI_Forced "Forzato"
#define MSGTR_NI_Detected "Rilevato"
#define MSGTR_NI_Message "%s formato file AVI NON-INTERLEAVED!\n"

#define MSGTR_UsingNINI "Uso di formato file AVI NON-INTERLEAVED corrotto!\n"
#define MSGTR_CouldntDetFNo "Impossibile determinare il numero di fotogrammi (per lo spostamento in valore assoluto)  \n"
#define MSGTR_CantSeekRawAVI "Impossibile spostarsi nei flussi .AVI grezzi! (richiesto un indice, prova con l\'opzione -idx!)  \n"
#define MSGTR_CantSeekFile "Impossibile spostarsi in questo file!  \n"

#define MSGTR_EncryptedVOB "File VOB criptato! Leggi il file DOCS/it/cd-dvd.html\n"

#define MSGTR_MOVcomprhdr "MOV: Il supporto delle intestazioni compresse richiede ZLIB!\n"
#define MSGTR_MOVvariableFourCC "MOV: Avvertimento! Rilevato FOURCC variabile!?\n"
#define MSGTR_MOVtooManyTrk "MOV: Avvertimento! troppe tracce!"
#define MSGTR_FoundAudioStream "==> Trovato flusso audio: %d\n"
#define MSGTR_FoundVideoStream "==> Trovato flusso video: %d\n"
#define MSGTR_DetectedTV "Ho trovato una TV! ;-)\n"
#define MSGTR_ErrorOpeningOGGDemuxer "Impossibile aprireil demuxer ogg\n"
#define MSGTR_ASFSearchingForAudioStream "ASF: sto cercandi il flusso audio (id:%d)\n"
#define MSGTR_CannotOpenAudioStream "Impossibile aprire il flusso audio: %s\n"
#define MSGTR_CannotOpenSubtitlesStream "Impossibile aprire il flusso dei sottotitoli: %s\n"
#define MSGTR_OpeningAudioDemuxerFailed "Errore nell'apertura del demuxer audio: %s\n"
#define MSGTR_OpeningSubtitlesDemuxerFailed "Errore nell'apertura del demuxer dei sottotitoli: %s\n"
#define MSGTR_TVInputNotSeekable "Impossibile spostarsi in un programma TV! (Probabilemnte lo spostamento sar? usato per cambiare canale ;)\n"
#define MSGTR_DemuxerInfoAlreadyPresent "Demuxer info %s already present!\n"
#define MSGTR_ClipInfo "Informazioni sulla clip: \n"

#define MSGTR_LeaveTelecineMode "\ndemux_mpg: Rilevato formato NTSC 30fps, cambio framerate.\n"
#define MSGTR_EnterTelecineMode "\ndemux_mpg: Rilevato formato NTSC 24fps progressivo, cambio framerate.\n"

// dec_video.c & dec_audio.c:
#define MSGTR_CantOpenCodec "impossibile aprire il codec\n"
#define MSGTR_CantCloseCodec "impossibile chiudere il codec\n"

#define MSGTR_MissingDLLcodec "ERRORE: Impossibile aprire il codec DirectShow richiesto: %s\n"
#define MSGTR_ACMiniterror "Impossibile caricare/inizializzare il codec audio Win32/ACM (manca il file DLL?)\n"
#define MSGTR_MissingLAVCcodec "Impossibile trovare il codec '%s' in libavcodec...\n"

#define MSGTR_MpegNoSequHdr "MPEG: FATAL: EOF mentre cercavo la sequenza di intestazione\n"
#define MSGTR_CannotReadMpegSequHdr "FATAL: Impossibile leggere la sequenza di intestazione!\n"
#define MSGTR_CannotReadMpegSequHdrEx "FATAL: Impossibile leggere l\'estensione della sequenza di intestazione!\n"
#define MSGTR_BadMpegSequHdr "MPEG: Sequenza di intestazione non valida!\n"
#define MSGTR_BadMpegSequHdrEx "MPEG: Estensione della sequenza di intestazione non valida!\n"

#define MSGTR_ShMemAllocFail "Impossibile allocare la memoria condivisa\n"
#define MSGTR_CantAllocAudioBuf "Impossibile allocare il buffer di uscita dell\'audio\n"

#define MSGTR_UnknownAudio "Formato audio sconosciuto/mancante, non uso l\'audio\n"

#define MSGTR_UsingExternalPP "[PP] Utilizzo un filtro di postprocessing esterno, max q = %d\n"
#define MSGTR_UsingCodecPP "[PP] Utilizzo il postprocessing del codec, max q = %d\n"
#define MSGTR_VideoAttributeNotSupportedByVO_VD "L'attributo video '%s' non ? supportato dal vo & vd selezionati! \n"
#define MSGTR_VideoCodecFamilyNotAvailableStr "La famiglia di codec video richiesta [%s] (vfm=%s) non ? disponibile (abilita in compilazione!)\n"
#define MSGTR_AudioCodecFamilyNotAvailableStr "La famiglia di codec audio richiesta [%s] (afm=%s) non ? disponibile (abilita in compilazione!)\n"
#define MSGTR_OpeningVideoDecoder "Apertura decoder video: [%s] %s\n"
#define MSGTR_OpeningAudioDecoder "Apertura decoder audio: [%s] %s\n"
#define MSGTR_UninitVideoStr "uninit video: %s  \n"
#define MSGTR_UninitAudioStr "uninit audio: %s  \n"
#define MSGTR_VDecoderInitFailed "Inizializazione VDecoder fallita :(\n"
#define MSGTR_ADecoderInitFailed "Inizializazione ADecoder fallita :(\n"
#define MSGTR_ADecoderPreinitFailed "Preinizializazione ADecoder fallita :(\n"
#define MSGTR_AllocatingBytesForInputBuffer "dec_audio: Alloco %d byte per il buffer di input\n"
#define MSGTR_AllocatingBytesForOutputBuffer "dec_audio: Alloco %d + %d = %d byte per il buffer di output\n"

// LIRC:
#define MSGTR_SettingUpLIRC "Configurazione del supporto per lirc...\n"
#define MSGTR_LIRCdisabled "Non potrai usare il tuo telecomando\n"
#define MSGTR_LIRCopenfailed "Apertura del supporto per lirc fallita!\n"
#define MSGTR_LIRCcfgerr "Fallimento nella lettura del file di configurazione di LIRC %s!\n"

// vf.c
#define MSGTR_CouldNotFindVideoFilter "Impossibile trovare il filtro video '%s'\n"
#define MSGTR_CouldNotOpenVideoFilter "Impossibile aprire il filtro video '%s'\n"
#define MSGTR_OpeningVideoFilter "Apertura filtro filter: "
#define MSGTR_CannotFindColorspace "Impossibile trovare un colorspace in comune, anche inserendo 'scale' :(\n"

// vd.c
#define MSGTR_CodecDidNotSet "VDec: il codec non ha impostato sh->disp_w and sh->disp_h, tento di aggirare il problema!\n"
#define MSGTR_VoConfigRequest "VDec: configurazione richiesta dal vo - %d x %d (csp preferito: %s)\n"
#define MSGTR_CouldNotFindColorspace "Impossibile trovare un colorspace adatto - riprovo con -vf scale...\n"
#define MSGTR_MovieAspectIsSet "Movie-Aspect ? %.2f:1 - riscalo per ottenere un rapporto corretto.\n"
#define MSGTR_MovieAspectUndefined "Movie-Aspect non definito - nessuna scalatura.\n"

// vd_dshow.c, vd_dmo.c
#define MSGTR_DownloadCodecPackage "Devi installare o aggiornare i codec binari.\nVai a http://mplayerhq.hu/homepage/dload.html\n"
#define MSGTR_DShowInitOK "INFO: Win32/DShow inizializzato correttamente.\n"
#define MSGTR_DMOInitOK "INFO: Win32/DMO inizializzato correttamente.\n"

// x11_common.c
#define MSGTR_EwmhFullscreenStateFailed "\nX11: Impossibile inviare l'evento schermo pieno EWMH!\n"

#define MSGTR_InsertingAfVolume "[Mixer] Nessun mixer hardware, filtro volume inserito automaticamente.\n"
#define MSGTR_NoVolume "[Mixer] Nessuna regolazione di volume disponibile.\n"

// ====================== GUI messages/buttons ========================

#ifdef HAVE_NEW_GUI

// --- labels ---
#define MSGTR_About "Informazioni su"
#define MSGTR_FileSelect "Seleziona il file..."
#define MSGTR_SubtitleSelect "Seleziona il sottotitolo..."
#define MSGTR_OtherSelect "Seleziona..."
#define MSGTR_AudioFileSelect "Seleziona canale audio esterno..."
#define MSGTR_FontSelect "Seleziona il carattere..."
#define MSGTR_PlayList "PlayList"
#define MSGTR_Equalizer "Equalizzatore"			 
#define MSGTR_SkinBrowser "Gestore Skin"
#define MSGTR_Network "Flusso dati dalla rete..."
#define MSGTR_Preferences "Preferenze"
#define MSGTR_AudioPreferences "Configurazione driver audio"
#define MSGTR_NoMediaOpened "nessun media aperto"
#define MSGTR_VCDTrack "Traccia VCD %d"
#define MSGTR_NoChapter "nessun capitolo"
#define MSGTR_Chapter "capitolo %d"
#define MSGTR_NoFileLoaded "nessun file caricato"
			 
// --- buttons ---
#define MSGTR_Ok "Ok"
#define MSGTR_Cancel "Annulla"
#define MSGTR_Add "Aggiungi"
#define MSGTR_Remove "Rimuovi"
#define MSGTR_Clear "Pulisci"
#define MSGTR_Config "Configura"
#define MSGTR_ConfigDriver "Configura driver"
#define MSGTR_Browse "Sfoglia"

// --- error messages ---
#define MSGTR_NEMDB "Mi dispiace, non c'? sufficiente memoria per il buffer di disegno."
#define MSGTR_NEMFMR "Mi dispiace, non c'? sufficiente memoria per visualizzare il menu."
#define MSGTR_IDFGCVD "Mi dispiace, non ho trovato un driver di output video compatibile con la GUI."
#define MSGTR_NEEDLAVCFAME "Mi dispiace, non puoi riprodurre file non-MPEG con il tuo dispositivo DXR3/H+ senza ricodificarli.\nAbilita lavc or fame nella finestra di configurazione DXR3/H+."

// --- skin loader error messages
#define MSGTR_SKIN_ERRORMESSAGE "[skin] errore nel file di configurazione della skin nella riga %d: %s"
#define MSGTR_SKIN_WARNING1 "[skin] avvertimento nel file di configurazione della skin nella riga %d: widget trovato ma non trovato prima di \"section\" (%s)"
#define MSGTR_SKIN_WARNING2 "[skin] avvertimento nel file di configurazione della skin nella riga %d: widget trovato ma non trovato prima di \"subsection\" (%s)"
#define MSGTR_SKIN_WARNING3 "[skin] avvertimento nel file di configurazione della skin nella riga %d: questa sottosezione non ? supportata da questo widget (%s)"
#define MSGTR_SKIN_BITMAP_16bit  "bitmap con profondit? di 16 bit o inferiore non supportata (%s).\n"
#define MSGTR_SKIN_BITMAP_FileNotFound  "file non trovato (%s)\n"
#define MSGTR_SKIN_BITMAP_BMPReadError "BMP, errore di lettura (%s)\n"
#define MSGTR_SKIN_BITMAP_TGAReadError "TGA, errore di lettura (%s)\n"
#define MSGTR_SKIN_BITMAP_PNGReadError "PNG, errore di lettura (%s)\n"
#define MSGTR_SKIN_BITMAP_RLENotSupported "RLE packed TGA non supportato (%s)\n"
#define MSGTR_SKIN_BITMAP_UnknownFileType "tipo di file sconosciuto (%s)\n"
#define MSGTR_SKIN_BITMAP_ConvertError "errore nella conversione da 24 bit a 32 bit (%s)\n"
#define MSGTR_SKIN_BITMAP_UnknownMessage "messaggio sconosciuto: %s\n"
#define MSGTR_SKIN_FONT_NotEnoughtMemory "memoria insufficiente\n"
#define MSGTR_SKIN_FONT_TooManyFontsDeclared "dichiarati troppi font\n"
#define MSGTR_SKIN_FONT_FontFileNotFound "file dei font non trovato\n"
#define MSGTR_SKIN_FONT_FontImageNotFound "file delle immagini dei font non trovato\n"
#define MSGTR_SKIN_FONT_NonExistentFontID "identificatore del font inesistente (%s)\n"
#define MSGTR_SKIN_UnknownParameter "parametro sconosciuto  (%s)\n"
#define MSGTR_SKIN_SKINCFG_SkinNotFound "Skin non trovata (%s).\n"
#define MSGTR_SKIN_SKINCFG_SkinCfgReadError "Errore nella lettura del file di configurazione della skin (%s).\n"
#define MSGTR_SKIN_LABEL "Skins:"

// --- gtk menus
#define MSGTR_MENU_AboutMPlayer "Informazione su MPlayer"
#define MSGTR_MENU_Open "Apri..."
#define MSGTR_MENU_PlayFile "Riproduci il file..."
#define MSGTR_MENU_PlayVCD "Riproduci il VCD..."
#define MSGTR_MENU_PlayDVD "Riproduci il DVD..."
#define MSGTR_MENU_PlayURL "Riproduci la URL..."
#define MSGTR_MENU_LoadSubtitle "Carica i sottotitoli..."
#define MSGTR_MENU_DropSubtitle "Elimina i sototitoli..."
#define MSGTR_MENU_LoadExternAudioFile "Carica file audio esterni..."
#define MSGTR_MENU_Playing "Riproduzione"
#define MSGTR_MENU_Play "Riproduci"
#define MSGTR_MENU_Pause "Pausa"
#define MSGTR_MENU_Stop "Interrompi"
#define MSGTR_MENU_NextStream "Stream successivo"
#define MSGTR_MENU_PrevStream "Stream precedente"
#define MSGTR_MENU_Size "Dimensione"
#define MSGTR_MENU_NormalSize "Dimensione normale"
#define MSGTR_MENU_DoubleSize "Dimensione doppia"
#define MSGTR_MENU_FullScreen "Schermo intero"
#define MSGTR_MENU_DVD "DVD"
#define MSGTR_MENU_VCD "VCD"
#define MSGTR_MENU_PlayDisc "Disco in riproduzione..."
#define MSGTR_MENU_ShowDVDMenu "Mostra il menu del DVD"
#define MSGTR_MENU_Titles "Titoli"
#define MSGTR_MENU_Title "Titolo %2d"
#define MSGTR_MENU_None "(niente)"
#define MSGTR_MENU_Chapters "Capitoli"
#define MSGTR_MENU_Chapter "Capitolo %2d"
#define MSGTR_MENU_AudioLanguages "Lingua dell\'audio"
#define MSGTR_MENU_SubtitleLanguages "Lingua dei sottotitoli"
#define MSGTR_MENU_PlayList "Playlist"
#define MSGTR_MENU_SkinBrowser "Skin browser"
#define MSGTR_MENU_Preferences "Preferenze"
#define MSGTR_MENU_Exit "Uscita..."
#define MSGTR_MENU_Mute "Mute"
#define MSGTR_MENU_Original "Originale"
#define MSGTR_MENU_AspectRatio "Aspetto"
#define MSGTR_MENU_AudioTrack "Traccia audio"
#define MSGTR_MENU_Track "Traccia %d"
#define MSGTR_MENU_VideoTrack "Traccia video"

// --- equalizer
#define MSGTR_EQU_Audio "Audio"
#define MSGTR_EQU_Video "Video"
#define MSGTR_EQU_Contrast "Contrasto: "
#define MSGTR_EQU_Brightness "Luminosit?: "
#define MSGTR_EQU_Hue "Tonalit?: "
#define MSGTR_EQU_Saturation "Saturazione: "
#define MSGTR_EQU_Front_Left "Anteriore Sinistro"
#define MSGTR_EQU_Front_Right "Anteriore Destro"
#define MSGTR_EQU_Back_Left "Posteriore Sinistro"
#define MSGTR_EQU_Back_Right "Posteriore Destro"
#define MSGTR_EQU_Center "Centro"
#define MSGTR_EQU_Bass "Bassi"
#define MSGTR_EQU_All "Tutti"
#define MSGTR_EQU_Channel1 "Canale 1:"
#define MSGTR_EQU_Channel2 "Canale 2:"
#define MSGTR_EQU_Channel3 "Canale 3:"
#define MSGTR_EQU_Channel4 "Canale 4:"
#define MSGTR_EQU_Channel5 "Canale 5:"
#define MSGTR_EQU_Channel6 "Canale 6:"

// --- playlist
#define MSGTR_PLAYLIST_Path "Percorso"
#define MSGTR_PLAYLIST_Selected "File selezionati"
#define MSGTR_PLAYLIST_Files "File"
#define MSGTR_PLAYLIST_DirectoryTree "albero delle directory"

// --- preferences
#define MSGTR_PREFERENCES_Misc "Varie"
#define MSGTR_PREFERENCES_None "Nessuno"
#define MSGTR_PREFERENCES_DriverDefault "Driver predefinito"
#define MSGTR_PREFERENCES_AvailableDrivers "Driver disponibili:"
#define MSGTR_PREFERENCES_DoNotPlaySound "Non riprodurre l'audio"
#define MSGTR_PREFERENCES_NormalizeSound "Normalizza l'audio"
#define MSGTR_PREFERENCES_EnEqualizer "Abilita l'equalizzatore"
#define MSGTR_PREFERENCES_ExtraStereo "Abilita l'extra stereo"
#define MSGTR_PREFERENCES_Coefficient "Coefficiente:"
#define MSGTR_PREFERENCES_AudioDelay "Ritatdo audio"
#define MSGTR_PREFERENCES_Audio "Audio"
#define MSGTR_PREFERENCES_DoubleBuffer "Abilita il doppio buffering"
#define MSGTR_PREFERENCES_DirectRender "Abilita il direct rendering"
#define MSGTR_PREFERENCES_FrameDrop "Abilita l'eliminazione dei frame"
#define MSGTR_PREFERENCES_HFrameDrop "Abilita l'eliminazione HARD dei frame (pericoloso)"
#define MSGTR_PREFERENCES_Flip "Ribalta l'immagine sottosopra"
#define MSGTR_PREFERENCES_Panscan "Panscan: "
#define MSGTR_PREFERENCES_Video "Video"
#define MSGTR_PREFERENCES_OSDTimer "Timer e indicatori"
#define MSGTR_PREFERENCES_OSDTimerPercentageTotalTime "Timer, percentuale e tempo totale"
#define MSGTR_PREFERENCES_OSDProgress "Solo progressbars"
#define MSGTR_PREFERENCES_Subtitle "Sottotitolo:"
#define MSGTR_PREFERENCES_SUB_Delay "Ritardo: "
#define MSGTR_PREFERENCES_SUB_FPS "FPS:"
#define MSGTR_PREFERENCES_SUB_POS "Posizione: "
#define MSGTR_PREFERENCES_SUB_AutoLoad "Disattiva il caricamento automatico dei sottotitoli"
#define MSGTR_PREFERENCES_SUB_Unicode "Sottotitoli unicode"
#define MSGTR_PREFERENCES_SUB_MPSUB "Converti i sottotitoli nel formato sottotitolo di MPlayer"
#define MSGTR_PREFERENCES_SUB_SRT "Converti i sottotitoli nel formato SubViewer (SRT) basato sul tempo"
#define MSGTR_PREFERENCES_SUB_Overlap "Attiva/Disattiva sovrapposizione sottotitoli"
#define MSGTR_PREFERENCES_Font "Carattere:"
#define MSGTR_PREFERENCES_Codecs "Codec e demuxer"
#define MSGTR_PREFERENCES_FontFactor "Font factor:"
#define MSGTR_PREFERENCES_PostProcess "Abilita postprocessing"
#define MSGTR_PREFERENCES_AutoQuality "Qualit? automatica: "
#define MSGTR_PREFERENCES_NI "Utilizza un analizzatore non-interleaved per i file AVI"
#define MSGTR_PREFERENCES_IDX "Ricostruisci l'indice, se necessario"
#define MSGTR_PREFERENCES_VideoCodecFamily "Famiglia codec video:"
#define MSGTR_PREFERENCES_AudioCodecFamily "Famiglia codec audio:"
#define MSGTR_PREFERENCES_FRAME_OSD_Level "Livello OSD"
#define MSGTR_PREFERENCES_FRAME_Subtitle "Sottotitoli"
#define MSGTR_PREFERENCES_FRAME_Font "Carattere"
#define MSGTR_PREFERENCES_FRAME_PostProcess "Postprocessing"
#define MSGTR_PREFERENCES_FRAME_CodecDemuxer "Codec e demuxer"
#define MSGTR_PREFERENCES_FRAME_Cache "Cache"
#define MSGTR_PREFERENCES_FRAME_Misc "Varie"
#define MSGTR_PREFERENCES_Audio_Device "Dispositivo:"
#define MSGTR_PREFERENCES_Audio_Mixer "Mixer:"
#define MSGTR_PREFERENCES_Audio_MixerChannel "Canale mixer:"
#define MSGTR_PREFERENCES_Message "Ricorda che devi riavviare la riproduzione affinch? alcune opzioni abbiano effetto!"
#define MSGTR_PREFERENCES_DXR3_VENC "Video encoder:"
#define MSGTR_PREFERENCES_DXR3_LAVC "Usa LAVC (FFmpeg)"
#define MSGTR_PREFERENCES_DXR3_FAME "Usa FAME"
#define MSGTR_PREFERENCES_FontEncoding1 "Unicode"
#define MSGTR_PREFERENCES_FontEncoding2 "Western European Languages (ISO-8859-1)"
#define MSGTR_PREFERENCES_FontEncoding3 "Western European Languages with Euro (ISO-8859-15)"
#define MSGTR_PREFERENCES_FontEncoding4 "Slavic/Central European Languages (ISO-8859-2)"
#define MSGTR_PREFERENCES_FontEncoding5 "Esperanto, Galician, Maltese, Turkish (ISO-8859-3)"
#define MSGTR_PREFERENCES_FontEncoding6 "Old Baltic charset (ISO-8859-4)"
#define MSGTR_PREFERENCES_FontEncoding7 "Cyrillic (ISO-8859-5)"
#define MSGTR_PREFERENCES_FontEncoding8 "Arabic (ISO-8859-6)"
#define MSGTR_PREFERENCES_FontEncoding9 "Modern Greek (ISO-8859-7)"
#define MSGTR_PREFERENCES_FontEncoding10 "Turkish (ISO-8859-9)"
#define MSGTR_PREFERENCES_FontEncoding11 "Baltic (ISO-8859-13)"
#define MSGTR_PREFERENCES_FontEncoding12 "Celtic (ISO-8859-14)"
#define MSGTR_PREFERENCES_FontEncoding13 "Hebrew charsets (ISO-8859-8)"
#define MSGTR_PREFERENCES_FontEncoding14 "Russian (KOI8-R)"
#define MSGTR_PREFERENCES_FontEncoding15 "Ukrainian, Belarusian (KOI8-U/RU)"
#define MSGTR_PREFERENCES_FontEncoding16 "Simplified Chinese charset (CP936)"
#define MSGTR_PREFERENCES_FontEncoding17 "Traditional Chinese charset (BIG5)"
#define MSGTR_PREFERENCES_FontEncoding18 "Japanese charsets (SHIFT-JIS)"
#define MSGTR_PREFERENCES_FontEncoding19 "Korean charset (CP949)"
#define MSGTR_PREFERENCES_FontEncoding20 "Thai charset (CP874)"
#define MSGTR_PREFERENCES_FontEncoding21 "Cyrillic Windows (CP1251)"
#define MSGTR_PREFERENCES_FontEncoding22 "Slavic/Central European Windows (CP1250)"
#define MSGTR_PREFERENCES_FontNoAutoScale "No autoscale"
#define MSGTR_PREFERENCES_FontPropWidth "Proporzionale alla larghezza del filmato"
#define MSGTR_PREFERENCES_FontPropHeight "Proporzionale all'altezza del filmato"
#define MSGTR_PREFERENCES_FontPropDiagonal "Proporzionale alla diagonale del filmato"
#define MSGTR_PREFERENCES_FontEncoding "Codifica:"
#define MSGTR_PREFERENCES_FontBlur "Blur:"
#define MSGTR_PREFERENCES_FontOutLine "Outline:"
#define MSGTR_PREFERENCES_FontTextScale "Text scale:"
#define MSGTR_PREFERENCES_FontOSDScale "Scala OSD:"
#define MSGTR_PREFERENCES_SubtitleOSD "Sottotitoli & OSD"
#define MSGTR_PREFERENCES_Cache "Cache on/off"
#define MSGTR_PREFERENCES_LoadFullscreen "Avvia a pieno schermo"
#define MSGTR_PREFERENCES_CacheSize "Dimensione cache: "
#define MSGTR_PREFERENCES_SaveWinPos "Salva la posizione della finestra"
#define MSGTR_PREFERENCES_XSCREENSAVER "Arresta XScreenSaver"
#define MSGTR_PREFERENCES_PlayBar "Attiva playbar"
#define MSGTR_PREFERENCES_AutoSync "AutoSync on/off"
#define MSGTR_PREFERENCES_AutoSyncValue "Autosync: "
#define MSGTR_PREFERENCES_CDROMDevice "Dispositivo CD-ROM:"
#define MSGTR_PREFERENCES_DVDDevice "Dispositivo DVD:"
#define MSGTR_PREFERENCES_FPS "FPS del filmato:"
#define MSGTR_PREFERENCES_ShowVideoWindow "Mostra la finestra video anche quando non ? attiva"

#define MSGTR_ABOUT_UHU "Lo sviluppo della GUI ? sponsorizzato da UHU Linux\n"
#define MSGTR_ABOUT_CoreTeam "   Team sviluppo MPlayer:\n"
#define MSGTR_ABOUT_AdditionalCoders "   Altri programmatori:\n"
#define MSGTR_ABOUT_MainTesters "   Tester principali:\n"

// --- messagebox
#define MSGTR_MSGBOX_LABEL_FatalError "Errore fatale!"
#define MSGTR_MSGBOX_LABEL_Error "Errore!"
#define MSGTR_MSGBOX_LABEL_Warning "Avvertimento!"

#endif
