//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
// .NAME qtPolygonEdgeOperationView - UI components for polygon edge operation View
// .SECTION Description
// .SECTION See Also
// qtSection

#ifndef qtPolygonEdgeOperationView_h
#define qtPolygonEdgeOperationView_h

#include "smtk/extension/qt/qtBaseView.h"

class qtPolygonEdgeOperationViewInternals;

class qtPolygonEdgeOperationView : public smtk::attribute::qtBaseView
{
  Q_OBJECT

public:
  static smtk::attribute::qtBaseView *createViewWidget(
         const smtk::attribute::ViewInfo &info);

  qtPolygonEdgeOperationView(const smtk::attribute::ViewInfo &info);
  virtual ~qtPolygonEdgeOperationView();

public slots:
  virtual void showAdvanceLevelOverlay(bool show);
  virtual void requestModelEntityAssociation();
  virtual void onShowCategory()
   { this->updateAttributeData(); }

protected:
  virtual void updateAttributeData();
  virtual void createWidget( );

private:

  qtPolygonEdgeOperationViewInternals *Internals;

}; // class


#endif
