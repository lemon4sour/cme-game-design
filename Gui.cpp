#include "Gui.h"

//Health Bar Variables
int iPercentage;
RECT rOutterHealthBar = { 796, 20, 996, 50 };
RECT rInnerHealthBar;
HBRUSH hRedBrush = CreateSolidBrush(RGB(153, 3, 3));
HBRUSH hGreyBrush = CreateSolidBrush(RGB(113, 112, 110));

//Timer Print Variables
DWORD iTimer = 0;
DWORD iTickStart = GetTickCount();
TCHAR cpTimerText[10];
RECT rTimerTextRect = {800, 50, 900, 70};

void PaintHealthBar(HWND hWindow, HDC hDC, int maxHealth, int currentHealth) {   
    iPercentage = currentHealth * 192 / maxHealth;
    
    SetRect(&rInnerHealthBar, 800, 24, 800 + iPercentage, 46);

    FillRect(hDC, &rOutterHealthBar, hGreyBrush);
    FillRect(hDC, &rInnerHealthBar, hRedBrush);
}

void PrintTime(HWND hWindow, HDC hDC) {
    if (GetTickCount() - iTickStart > 1000) {
        iTickStart = GetTickCount();
        iTimer++;
    }

    SetBkColor(hDC, RGB(41, 35, 35));
    SetTextColor(hDC, RGB(255, 255, 255));
    wsprintf(cpTimerText, TEXT("Timer: %d"), iTimer);
    DrawText(hDC, cpTimerText, -1, &rTimerTextRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
}

ElementQueue::ElementQueue(HWND hWindow, HDC hDC, Bitmap* earth, Bitmap* fire, Bitmap* water, Bitmap* air) {
    m_hWindow = hWindow;
    m_hDC = hDC;

    m_pElementBitmaps[0] = earth;
    m_pElementBitmaps[1] = fire;
    m_pElementBitmaps[2] = water;
    m_pElementBitmaps[3] = air;
}

void ElementQueue::FillRandom() {
    for (int i = 0; i < iQueueSize; i++) {
        AddRandomElement();
    }
}

void ElementQueue::AddRandomElement() {
    if (m_qElementBitmaps.size() >= iQueueSize)
        return;

    int randomElement = rand() % 4;
    m_qElementBitmaps.push(m_pElementBitmaps[randomElement]);
    m_qElementTypes.push(static_cast<ElementType>(randomElement));
}

void ElementQueue::AddElement(ElementType type) {
    if (m_qElementBitmaps.size() <= iQueueSize) {
        m_qElementBitmaps.push(m_pElementBitmaps[type]);
        m_qElementTypes.push(type);
    }
}

void ElementQueue::AddElement(int type) {
    AddElement(static_cast<ElementType>(type));
}

void ElementQueue::DrawQueue() {
    FillRect(m_hDC, &m_rOutterQueue, hGreyBrush);
    
    if (!m_qElementBitmaps.empty()) {
        std::queue<Bitmap*> tempQueue;
        Bitmap* tempBitmap;


        int position = 205;
        while (!m_qElementBitmaps.empty()) {
            tempBitmap = m_qElementBitmaps.front();
            tempBitmap->Draw(m_hDC, 821, position);
            tempQueue.push(tempBitmap);
            m_qElementBitmaps.pop();
            position += 69;
        }

        while (!tempQueue.empty()) {
            m_qElementBitmaps.push(tempQueue.front());
            tempQueue.pop();
        }
    }
}

ElementType ElementQueue::UseElement() {
    
    if (m_qElementBitmaps.empty()) {
        return static_cast<ElementType>(-1);
    }

    ElementType type = m_qElementTypes.front();
    m_qElementTypes.pop();
    m_qElementBitmaps.pop();
    

    return type;
}