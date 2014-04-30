/*=========================================================================

  Module:    qtUIManager.h,v

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME qtUIManager - a user interface manager.
// .SECTION Description
// .SECTION See Also

#ifndef __smtk_attribute_qtUIManager_h
#define __smtk_attribute_qtUIManager_h

#include "smtk/Qt/qtItem.h"
#include "smtk/attribute/Manager.h"
#include <QFont>
#include <QColor>
#include <QDoubleValidator>
#include <QTextEdit>
#include <QMap>

class QTableWidget;
class QTableWidgetItem;
class QComboBox;

namespace smtk
{
  namespace attribute
  {
    class qtItem;
    class qtFileItem;
    class qtRootView;
    class qtAttributeView;
    class qtBaseView;
    class qtInstancedView;
    class qtModelEntityView;
    class qtSimpleExpressionView;
    class qtGroupView;

    class QTSMTK_EXPORT qtUIManager : public QObject
    {

    Q_OBJECT

    friend class qtRootView;

    public:
      qtUIManager(smtk::attribute::Manager &manager);
      virtual ~qtUIManager();

      void initializeUI(QWidget* pWidget, bool useInternalFileBrowser=false);
      void initializeView(QWidget* pWidget, smtk::view::BasePtr base,
        bool useInternalFileBrowser=true);
      smtk::attribute::Manager* attManager() const
        {return &this->m_AttManager;}

      // Description:
      // Set/Get the color used for indicating items with default values
      void setDefaultValueColor(const QColor &color);
      QColor defaultValueColor() const
      {return this->DefaultValueColor;}
      void setInvalidValueColor(const QColor &color);
      QColor invalidValueColor() const
      {return this->InvalidValueColor;}

      // Description:
      // Set the advanced values font to be bold and/or italic
      void setAdvancedBold(bool b)
      {this->AdvancedBold = b;}
      bool advancedBold()
      {return this->AdvancedBold;}
      void setAdvancedItalic(bool i)
      {this->AdvancedItalic = i;}
      bool advancedItalic()
      {return this->AdvancedItalic;}

      qtRootView* rootView()
        {return this->RootView;}
      static QString clipBoardText();
      static void setClipBoardText(QString& text);
      std::string currentCategory();
      bool categoryEnabled();
      void clearRoot();

      bool passAdvancedCheck(bool advancedAtt);
      bool passAttributeCategoryCheck(smtk::attribute::ConstDefinitionPtr AttDef);
      bool passItemCategoryCheck(smtk::attribute::ConstItemDefinitionPtr ItemDef);
      bool passCategoryCheck(const std::set<std::string> & categories);

      const QFont& advancedFont()
        {return this->advFont;}

      void setWidgetColor(QWidget *widget, const QColor &color);
      bool getExpressionArrayString(
        smtk::attribute::GroupItemPtr dataItem, QString& strValues);

    static void updateArrayTableWidget(smtk::attribute::GroupItemPtr dataItem,
                                       QTableWidget* widget);
    static void updateTableColRows(smtk::attribute::ItemPtr dataItem,
      int col, QTableWidget* widget);

    static void updateArrayDataValue(smtk::attribute::GroupItemPtr dataItem,
                                     QTableWidgetItem* item);
    static void addNewTableValues(smtk::attribute::GroupItemPtr dataItem,
      QTableWidget* table, double* vals, int numVals);
    static void removeSelectedTableValues(
      smtk::attribute::GroupItemPtr dataItem, QTableWidget* table);

    bool updateTableItemCheckState(
      QTableWidgetItem* labelitem, smtk::attribute::ItemPtr attItem);

    virtual QWidget* createInputWidget(smtk::attribute::ItemPtr,
                                       int elementIdx, QWidget* pWidget,
                                       qtBaseView* bview);
    virtual QWidget* createEditBox(smtk::attribute::ItemPtr,
                                   int elementIdx, QWidget* pWidget, qtBaseView* bview);
//    virtual QWidget* createComboBox(smtk::attribute::ItemPtr,
//                                    int elementIdx, QWidget* pWidget,
//                                   qtBaseView* bview);
    virtual QWidget* createExpressionRefWidget(smtk::attribute::ItemPtr,
                                               int elementIdx,QWidget* pWidget, qtBaseView* bview);

    virtual int getWidthOfAttributeMaxLabel(smtk::attribute::DefinitionPtr def,
                                     const QFont &font);
    virtual int getWidthOfItemsMaxLabel(
      const QList<smtk::attribute::ItemDefinitionPtr>& itemDefs,
      const QFont &font);

#ifdef _WIN32
    #define LINE_BREAKER_STRING "\n";
#else
    #define LINE_BREAKER_STRING "\r";
#endif

    public slots:
      void onFileItemCreated(smtk::attribute::qtFileItem*);
//      void onComboIndexChanged();
      void onExpressionReferenceChanged();
      void updateModelViews();
      void updateModelViews(qtGroupView*);
      void onTextEditChanged();
      void onLineEditChanged();
      void onLineEditFinished();
      void onInputValueChanged(QObject*);
      void onViewUIModified(smtk::attribute::qtBaseView*, smtk::attribute::ItemPtr);

    signals:
      void fileItemCreated(smtk::attribute::qtFileItem* fileItem);
      void uiChanged(smtk::attribute::qtBaseView*, smtk::attribute::ItemPtr);

    protected:
      void processAttributeView(qtAttributeView* v);
      void processInstancedView(qtInstancedView* v);
      void processModelEntityView(qtModelEntityView* v);
      void processSimpleExpressionView(qtSimpleExpressionView* v);
      void processGroupView(qtGroupView* v);
      void processBasicView(qtBaseView* v);

      qtBaseView* createView(smtk::view::BasePtr smtkView, QWidget *pWidget);

   private:
      qtRootView* RootView;
      QFont advFont;
      QColor DefaultValueColor;
      QColor InvalidValueColor;
      bool AdvancedBold; // true by default
      bool AdvancedItalic; // false by default

      smtk::attribute::Manager &m_AttManager;
      bool m_useInternalFileBrowser;
      // map for <Definition, its longest item label>
      // The longest label is used as a hint when aligning all item labels
      QMap<smtk::attribute::DefinitionPtr, std::string> Def2LongLabel;
      void findDefinitionsLongLabels();
      void findDefinitionLongLabel(smtk::attribute::DefinitionPtr def, std::string &labelText);
      void getItemsLongLabel(
        const QList<smtk::attribute::ItemDefinitionPtr>& itemDefs,
        std::string &labelText);

    }; // class

    //A sublcass of QDoubleValidator to fixup input outside of range
    class QTSMTK_EXPORT qtDoubleValidator : public QDoubleValidator
    {
      Q_OBJECT
    public:
        qtDoubleValidator(QObject * parent);
        virtual void fixup(QString &input) const;

        void setUIManager(qtUIManager* uiman);
    private:
      qtUIManager* UIManager;
    };

    //A sublcass of QIntValidator to fixup input outside of range
    class QTSMTK_EXPORT qtIntValidator : public QIntValidator
      {
      Q_OBJECT
      public:
        qtIntValidator(QObject * parent);
        virtual void fixup(QString &input) const;

        void setUIManager(qtUIManager* uiman);
      private:
        qtUIManager* UIManager;
      };

    //A sublcass of QTextEdit to give initial sizehint
    class QTSMTK_EXPORT qtTextEdit : public QTextEdit
      {
      Q_OBJECT
      public:
        qtTextEdit(QWidget * parent);
        virtual QSize sizeHint() const;
      };

  }; // namespace attribute
}; // namespace smtk

#endif
