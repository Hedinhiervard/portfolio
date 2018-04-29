// @flow

import type { OptionsType } from 'frontend/types/Option'
import type { Action } from 'frontend/types/actions'

export type OptionsState = OptionsType;

const initialState: OptionsState = []

export default (state: OptionsState = initialState, action: Action): OptionsState => {
    switch (action.type) {
    case 'GET_OPTIONS_SUCCESS':
        return action.options
    case 'POST_OPTIONS_SUCCESS':
        return [
            ...state,
            action.option,
        ]
    case 'DELETE_OPTION_SUCCESS':
        const id = action.deletedId
        return state.filter(o => o._id !== id)
    case 'COMMIT_OPTION_SUCCESS':
    {
        const id = action._id
        const newOption = action.newOption
        return state.map(option =>
            option._id === id
                ? newOption
                : option)
    }
    default:
        return state
    }
}
