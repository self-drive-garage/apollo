import { makeStyles } from '@dreamview/dreamview-theme';

export default makeStyles((theme) => ({
    'viz-container': {
        padding: theme.tokens.padding.speace0,
        minWidth: '244px',
        height: '100%',
        position: 'relative',
    },
    'viz-context': {
        width: '100%',
        height: '100%',
    },
    'routing-editing-context': {
        width: '100%',
        height: '100%',
    },
    'web-gl': {
        width: '100%',
        height: '100%',
    },
    'layer-menu-container': {
        padding: theme.tokens.padding.speace0,
        width: '532px',
        height: '332px',
        margin: '-5px -10px',
        background: theme.components.layerMenu.bgColor,
        borderRadius: '8px',
    },
    'layer-menu-header': {
        lineHeight: '40px',
        height: '40px',
        borderBottom: theme.components.layerMenu.headBorder,
        color: theme.components.layerMenu.headColor,
    },
    'layer-menu-header-left': {
        paddingLeft: '24px',
        paddingRight: theme.tokens.padding.speace2,
        fontFamily: 'PingFangSC-Medium',
        fontSize: '16px',
        fontWeight: '500',
    },
    'layer-menu-header-right': {
        float: 'right',
        marginTop: '-46px',
        marginRight: theme.tokens.padding.speace2,
    },
    'layer-menu-header-reset-btn': {
        lineHeight: '28px',
        marginTop: '10px',
        borderRadius: '6px',
        border: theme.components.layerMenu.headResetBtnBorderColor,
        padding: '0px 10px',
        cursor: 'pointer',
        color: theme.components.layerMenu.headResetBtnColor,
    },
    'layer-menu-content': {
        display: 'flex',
        flexDirection: 'row',
        padding: '12px 0 12px 16px',
    },
    'layer-menu-content-left': {
        borderRight: theme.components.layerMenu.headBorder,
        width: '126px',
        color: theme.components.layerMenu.tabColor,
    },
    'layer-menu-content-left-li': {
        paddingLeft: '16px',
        width: '110px',
        height: '32px',
        lineHeight: '32px',
        cursor: 'pointer',
        marginBottom: '6px',
        color: theme.components.layerMenu.tabColor,
        fontWeight: '400',
        fontFamily: ' PingFangSC-Regular',
    },
    'li-active': {
        color: '#FFFFFF',
        background: theme.components.layerMenu.activeTabBgColor,
        borderRadius: '6px',
    },
    'layer-menu-content-right': {
        paddingLeft: theme.tokens.padding.speace2,
        flex: 1,
        overflowY: 'auto',
        height: '268px',
    },
    'layer-menu-content-right-li': {
        '&:nth-of-type(2n + 1)': {
            minWidth: '150px',
        },
        '&[data-width="max"]': {
            width: '216px',
        },
        '&[data-width="min"]': {
            width: '150px',
        },
        '&[data-width="minMax"]': {
            minWidth: '150px',
        },
        height: '34px',
        lineHeight: '34px',
        display: 'inline-block',
    },
    'layer-menu-horizontal-line': {
        height: '1px',
        background: theme.components.layerMenu.headBorderColor,
        margin: '8px 12px 8px 0',
    },
    'layer-menu-content-right-switch': {
        paddingLeft: theme.tokens.padding.speace,
        paddingRight: theme.tokens.padding.speace,
    },
    'layer-menu-content-right-label': {
        color: theme.components.layerMenu.labelColor,
        fontWeight: '400',
        verticalAlign: 'middle',
        fontFamily: ' PingFangSC-Regular',
    },
    'viz-rend-fps-item': {
        position: 'absolute',
        width: 192,
        height: 32,
        color: '#A6B5CC',
        top: 34,
        left: 274,
    },
    'viz-rend-fps-item-hide': {
        position: 'absolute',
        // cursor: 'pointer',
        width: '32px',
        height: '32px',
        top: 18,
        left: 224,
    },
    'viz-btn-container': {
        position: 'absolute',
        bottom: '44px',
        right: theme.tokens.padding.speace2,
        display: 'flex',
        flexDirection: 'column',
    },
    'viz-btn-item': {
        display: 'inline-block',
        cursor: 'pointer',
        textAlign: 'center',
        width: '32px',
        height: '32px',
        lineHeight: '32px',
        background: theme.components.layerMenu.menuItemBg,
        boxShadow: theme.components.layerMenu.boxShadow,
        borderRadius: '6px',
        marginTop: '12px',
        fontSize: '16px',
        color: theme.tokens.colors.fontColor4,
    },
    'viz-help-btn-item': {
        display: 'inline-block',
        cursor: 'pointer',
        textAlign: 'center',
        width: '32px',
        height: '32px',
        lineHeight: '32px',
        background: theme.components.panelBase.functionRectBgColor,
        borderRadius: '6px',
        marginTop: '12px',
        fontSize: '16px',
        color: theme.components.panelBase.functionRectColor,
    },
    'viz-btn-item-flex': {
        display: 'flex',
        flexDirection: 'column',
        alignItems: 'center',
        justifyContent: 'center',
        padding: theme.tokens.padding.speace,
        height: 'auto',
    },
    'view-menu-item': {
        height: '32px',
        lineHeight: '32px',
        color: theme.components.layerMenu.color,
        cursor: 'pointer',
        paddingLeft: '24px',
        fontFamily: 'PingFangSC-Regular',
        fontSize: '14px',
        fontWeight: '400',
    },
    'view-menu-scale-btn-container': {
        marginTop: '12px',
        width: '32px',
        height: '56px',
        background: theme.components.layerMenu.menuItemBg,
        borderRadius: '6px',
        fontSize: '20px',
        fontWeight: '600',
        display: 'flex',
        flexDirection: 'column',
        justifyContent: 'center',
        alignItems: 'center',
        boxShadow: theme.components.layerMenu.menuItemBoxShadow,
    },
    'view-menu-btn-item-only': {
        cursor: 'pointer',
        width: '32px',
        height: '32px',
        background: theme.components.layerMenu.menuItemBg,
        boxShadow: theme.components.layerMenu.menuItemBoxShadow,
        borderRadius: '6px',
        marginTop: '10px',
        ...theme.util.flexCenterCenter,
    },
    'view-menu-btn-item': {
        cursor: 'pointer',
    },
    'view-menu-container': {
        width: '158px',
        margin: '-5px -10px',
        borderRadius: '8px',
        background: theme.components.layerMenu.bgColor,
    },
    'view-menu-header': {
        height: '40px',
        color: theme.components.layerMenu.headColor,
        fontSize: '16px',
        fontWeight: '500',
        fontFamily: 'PingFangSC-Medium',
        lineHeight: '40px',
        paddingLeft: '24px',
        marginBottom: '8px',
        borderBottom: theme.components.layerMenu.headBorder,
    },
    'view-menu-active': {
        background: theme.components.layerMenu.activeTabBgColor,
        color: 'white',
    },
    'panel-desc-item': {
        height: '76px',
        lineHeight: '76px',

        display: 'flex',
        justifyContent: 'center',
        '&:not(:last-of-type)': {
            borderBottom: `1px solid ${theme.tokens.divider.color.light}`,
        },
    },
    'panel-desc-item-left': {
        width: '170px',
    },
    'panel-desc-item-right': {
        width: '584px',
        height: '22px',
        color: theme.tokens.colors.fontColor5,
        fontWeight: 400,
        fontFamily: 'PingFangSC-Regular',
    },
    'view-btn-container': {
        position: 'absolute',
        bottom: '44px',
        right: '24px',
    },
    'view-ope-container': {
        position: 'absolute',
        top: '25px',
        right: '24px',
    },
}));
