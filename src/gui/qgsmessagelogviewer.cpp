/***************************************************************************
                          qgsmessagelogviewer.cpp  -  description
                             -------------------
    begin                : October 2011
    copyright            : (C) 2011 by Juergen E. Fischer
    email                : jef at norbit dot de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsmessagelogviewer.h"
#include "qgsmessagelog.h"
#include "qgsapplication.h"

#include <QFile>
#include <QDateTime>
#include <QTableWidget>
#include <QToolButton>
#include <QStatusBar>
#include <QToolTip>
#include <QDockWidget>
#include <QPlainTextEdit>
#include <QScrollBar>

static QIcon icon( QString icon )
{
  // try active theme
  QString path = QgsApplication::activeThemePath();
  if ( QFile::exists( path + icon ) )
    path += icon;
  else
    path = QgsApplication::defaultThemePath() + icon;

  return QIcon( path );
}

QgsMessageLogViewer::QgsMessageLogViewer( QStatusBar *statusBar, QWidget *parent, Qt::WindowFlags fl )
    : QDialog( parent, fl )
    , mButton( 0 )
    , mShowToolTips( true )
{
  setupUi( this );

  connect( QgsMessageLog::instance(), SIGNAL( messageReceived( QString, QString, QgsMessageLog::MessageLevel ) ),
           this, SLOT( logMessage( QString, QString, QgsMessageLog::MessageLevel ) ) );

  if ( statusBar )
  {
    mButton = new QToolButton( parent );
    mButton->setObjectName( "mMessageLogViewerButton" );
    mButton->setMaximumWidth( 20 );
    mButton->setMaximumHeight( 20 );
    mButton->setIcon( icon( "/mIconWarn.png" ) );
#ifndef ANDROID
    mButton->setToolTip( tr( "No messages." ) );
#endif
    mButton->setCheckable( true );
    connect( mButton, SIGNAL( toggled( bool ) ), this, SLOT( buttonToggled( bool ) ) );
    connect( mButton, SIGNAL( destroyed() ), this, SLOT( buttonDestroyed() ) );
    statusBar->addPermanentWidget( mButton, 0 );
  }

  connect( tabWidget, SIGNAL( tabCloseRequested( int ) ), this, SLOT( closeTab( int ) ) );
}

QgsMessageLogViewer::~QgsMessageLogViewer()
{
}

void QgsMessageLogViewer::hideEvent( QHideEvent * )
{
  if ( mButton )
  {
    mButton->setChecked( false );
  }
}

void QgsMessageLogViewer::showEvent( QShowEvent * )
{
  if ( mButton )
  {
    mButton->setChecked( true );
  }
}

void QgsMessageLogViewer::buttonToggled( bool checked )
{
  QWidget *w = qobject_cast<QDockWidget *>( parent() );

  if ( !w )
    w = this;

  if ( checked )
    w->show();
  else
    w->hide();
}

void QgsMessageLogViewer::buttonDestroyed()
{
  mButton = 0;
}

void QgsMessageLogViewer::logMessage( QString message, QString tag, QgsMessageLog::MessageLevel level )
{
#ifdef ANDROID
  mButton->setToolTip( tr( "Message(s) logged." ) );
#endif

  if ( !isVisible() && level > QgsMessageLog::INFO )
  {
    mButton->show();
    if ( mShowToolTips )
      QToolTip::showText( mButton->mapToGlobal( QPoint( 0, 0 ) ), mButton->toolTip() );
  }

  if ( tag.isNull() )
    tag = tr( "General" );

  int i;
  for ( i = 0; i < tabWidget->count() && tabWidget->tabText( i ) != tag; i++ )
    ;

  QPlainTextEdit *w;
  if ( i < tabWidget->count() )
  {
    w = qobject_cast<QPlainTextEdit *>( tabWidget->widget( i ) );
    tabWidget->setCurrentIndex( i );
  }
  else
  {
    w = new QPlainTextEdit( this );
    w->setReadOnly( true );
    tabWidget->addTab( w, tag );
    tabWidget->setCurrentIndex( tabWidget->count() - 1 );
  }

  QString prefix = QString( "%1\t%2\t" )
                   .arg( QDateTime::currentDateTime().toString( Qt::ISODate ) )
                   .arg( level );
  w->appendPlainText( message.prepend( prefix ).replace( "\n", "\n\t\t\t" ) );
  w->verticalScrollBar()->setValue( w->verticalScrollBar()->maximum() );
}

void QgsMessageLogViewer::closeTab( int index )
{
  tabWidget->removeTab( index );
}
