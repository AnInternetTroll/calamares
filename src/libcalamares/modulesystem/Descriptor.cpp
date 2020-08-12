/* === This file is part of Calamares - <https://github.com/calamares> ===
 *
 *   SPDX-FileCopyrightText: 2020 Adriaan de Groot <groot@kde.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "Descriptor.h"

#include "utils/Logger.h"
#include "utils/Variant.h"

namespace Calamares
{
namespace ModuleSystem
{

const NamedEnumTable< Type >&
typeNames()
{
    // *INDENT-OFF*
    // clang-format off
    static const NamedEnumTable< Type > table{
        { QStringLiteral( "job" ), Type::Job },
        { QStringLiteral( "view" ), Type::View },
        { QStringLiteral( "viewmodule" ), Type::View },
        { QStringLiteral( "jobmodule" ), Type::Job }
    };
    // *INDENT-ON*
    // clang-format on
    return table;
}

const NamedEnumTable< Interface >&
interfaceNames()
{
    // *INDENT-OFF*
    // clang-format off
    static const NamedEnumTable< Interface > table {
        { QStringLiteral("process"), Interface::Process },
        { QStringLiteral("qtplugin"), Interface::QtPlugin },
        { QStringLiteral("python"), Interface::Python },
        { QStringLiteral("pythonqt"), Interface::PythonQt }
    };
    // *INDENT-ON*
    // clang-format on
    return table;
}

Descriptor::Descriptor() {}

Descriptor
Descriptor::fromDescriptorData( const QVariantMap& moduleDesc )
{
    Descriptor d;

    cDebug() << moduleDesc;

    {
        bool typeOk = false;
        QString typeValue = moduleDesc.value( "type" ).toString();
        Type t = typeNames().find( typeValue, typeOk );
        if ( !typeOk )
        {
            cWarning() << "Module descriptor contains invalid *type*" << typeValue;
        }

        bool interfaceOk = false;
        QString interfaceValue = moduleDesc.value( "interface" ).toString();
        Interface i = interfaceNames().find( interfaceValue, interfaceOk );
        if ( !interfaceOk )
        {
            cWarning() << "Module descriptor contains invalid *interface*" << interfaceValue;
        }

        d.m_name = moduleDesc.value( "name" ).toString();
        if ( typeOk && interfaceOk && !d.m_name.isEmpty() )
        {
            d.m_type = t;
            d.m_interface = i;
            d.m_isValid = true;
        }
    }
    if ( !d.m_isValid )
    {
        return d;
    }

    d.m_isEmergeny = CalamaresUtils::getBool( moduleDesc, "emergency", false );
    d.m_hasConfig = !CalamaresUtils::getBool( moduleDesc, "noconfig", false );  // Inverted logic during load
    d.m_requiredModules = CalamaresUtils::getStringList( moduleDesc, "requiredModules" );

    QStringList consumedKeys { "type", "interface", "name", "emergency", "noconfig", "requiredModules" };

    QStringList superfluousKeys;
    for ( auto kv = moduleDesc.keyBegin(); kv != moduleDesc.keyEnd(); ++kv )
    {
        if ( !consumedKeys.contains( *kv ) )
        {
            superfluousKeys << *kv;
        }
    }
    if ( !superfluousKeys.isEmpty() )
    {
        cWarning() << "Module descriptor contains extra keys:" << Logger::DebugList( superfluousKeys );
        d.m_isValid = false;
    }

    return d;
}

}  // namespace ModuleSystem
}  // namespace Calamares
