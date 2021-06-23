#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QWidget>
#include <QMenu>

class QMenu;


class ContextMenu : public QWidget
{
    Q_OBJECT

public:
    explicit ContextMenu(QWidget *parent = nullptr);
    ~ContextMenu();


    void ContextMenuEvent(QContextMenuEvent* e);

private:
    QMenu *context_menu;
};

#endif // CONTEXTMENU_H
