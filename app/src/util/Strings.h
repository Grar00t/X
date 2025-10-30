#pragma once
#include <QString>
namespace Str {
    inline QString trimLines(const QString& s){ auto out = s.trimmed(); out.replace("\r\n","\n"); return out; }
}
