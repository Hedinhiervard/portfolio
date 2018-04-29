// @flow
import * as React from 'react'
import { bindActionCreators } from 'redux';
import { connect } from 'react-redux';
import * as acLogin from 'frontend/action_creators/login'

import { Form, Input, SubmitButton } from 'frontend/components/UI/Forms'
import { Validatable } from 'frontend/components/Validatable'
import { Link } from 'react-router-dom'
import { Container, Row, Col, AlignRight, AlignLeft } from 'frontend/components/UI/Grid'
import { hvCenteredStyle } from 'frontend/components/UI'

import type { State } from 'frontend/reducers'
import type { Dispatch } from 'frontend/types/redux'

export class LoginForm extends Validatable<{|
    acSubmit: (string, string) => any,
|}, {
    email: string,
    password: string,
}> {
    constructor() {
        super()

        this.state = {
            email: '',
            password: '',
        }

        this.validationRules = {
            email: this.defaultRules.email,
            password: this.defaultRules.password,
        }
    }

    render() {
        const { email, password } = this.state
        const { acSubmit } = this.props
        const validationErrors = this.validationErrors

        return (
            <Form
                acSubmit = { () => {
                    this.validate() && acSubmit(email, password)
                }}
            >
                <Container>
                    <Row style = { hvCenteredStyle }>
                        Login
                    </Row>
                    <Row>
                        <Col>
                            <AlignRight>
                                Email
                            </AlignRight>
                        </Col>
                        <Col>
                            <AlignLeft>
                                <Input
                                    type="text"
                                    value= { email }
                                    acChange = { newValue => this.setState({ email: newValue }) }
                                />
                            </AlignLeft>
                        </Col>
                        <Col>
                            { validationErrors.email }
                        </Col>
                    </Row>
                    <Row>
                        <Col>
                            <AlignRight>
                                Password
                            </AlignRight>
                        </Col>
                        <Col>
                            <AlignLeft>
                                <Input
                                    type="password"
                                    value= { password }
                                    acChange = { newValue => this.setState({ password: newValue }) }
                                />
                            </AlignLeft>
                        </Col>
                        <Col>
                            { validationErrors.password }
                        </Col>
                    </Row>
                    <Row style = { hvCenteredStyle }>
                        <SubmitButton text = "SUBMIT"/>
                    </Row>
                    <Row>
                        <Col>
                            <AlignRight>
                                <Link to='/resetPassword'>
                                    Forgot password?
                                </Link>
                            </AlignRight>
                        </Col>
                        <Col>
                            <AlignLeft>
                                <Link to='/resendEmailConfirmation'>
                                    Did not get confirmation email?
                                </Link>
                            </AlignLeft>
                        </Col>
                    </Row>
                </Container>

            </Form>
        );
    }
}

const mapStateToProps = (state: State) => ({
});

//
const mapDispatchToProps = (dispatch: Dispatch) => ({
    acSubmit: bindActionCreators(acLogin.acLoginSubmit, dispatch),
});

export default connect(
    mapStateToProps,
    mapDispatchToProps
)(LoginForm)
